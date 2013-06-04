/**
 * @package     platypus-phyloinformary
 * @brief       Parse data of various formats and produce corresponding data
 *              objects using the Nexus Class Library (NCL).
 * @author      Jeet Sukumaran
 * @copyright   Copyright (C) 2013 Jeet Sukumaran.
 *              This file is part of "platypus-phyloinformary".
 *              "platypus-phyloinformary" is free software: you can
 *              redistribute it and/or modify it under the terms of the GNU
 *              General Public License as published by the Free Software
 *              Foundation, either version 3 of the License, or (at your
 *              option) any later version.  "platypus-phyloinformary" is
 *              distributed in the hope that it will be useful, but WITHOUT ANY
 *              WARRANTY; without even the implied warranty of MERCHANTABILITY
 *              or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *              Public License for more details.  You should have received a
 *              copy of the GNU General Public License along with
 *              "platypus-phyloinformary".  If not, see
 *              <http://www.gnu.org/licenses/>.
 */

#ifndef PLATYPUS_PARSE_NCLREADER_HPP
#define PLATYPUS_PARSE_NCLREADER_HPP

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include <map>
#include <ncl/nxsmultiformat.h>
#include "../base/base_reader.hpp"

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// NclTreeReader

template <typename TreeT, class EdgeLengthT=double>
class NclTreeReader : public BaseTreeReader<TreeT> {

    public:
        typedef TreeT                          tree_type;
        typedef typename tree_type::node_type  tree_node_type;
        typedef typename tree_type::value_type tree_value_type;

    public:

        NclTreeReader() { }
        unsigned long read(
                std::istream & src,
                const std::function<tree_type & ()> & get_new_tree_reference,
                const std::string & format="nexus",
                unsigned long tree_limit=0) {
            return this->parse_stream(src, get_new_tree_reference, format, tree_limit);
        }

        unsigned long read(
                std::istream && src,
                const std::function<tree_type & ()> & get_new_tree_reference,
                const std::string & format="nexus",
                unsigned long tree_limit=0) {
            return this->parse_stream(src, get_new_tree_reference, format, tree_limit);
        }

    protected:

        unsigned long parse_stream(
                std::istream & src,
                const std::function<tree_type & ()> & get_new_tree_reference,
                unsigned long tree_limit=0) override {
            return this->parse_stream(src, get_new_tree_reference,"nexus", tree_limit);
        }

        unsigned long parse_stream(
                std::istream & src,
                const std::function<tree_type & ()> & get_new_tree_reference,
                const std::string & format,
                unsigned long tree_limit=0) {
            MultiFormatReader reader(-1, NxsReader::IGNORE_WARNINGS);
            reader.SetWarningOutputLevel(NxsReader::AMBIGUOUS_CONTENT_WARNING);
            reader.SetCoerceUnderscoresToSpaces(false);
            const char * format_cstr = nullptr;
            if (format == "newick") {
                format_cstr = "relaxedphyliptree";
            } else {
                format_cstr = format.c_str();
            }
            reader.ReadStream(src, format_cstr);
            unsigned num_taxa_blocks = reader.GetNumTaxaBlocks();
            NxsTaxaBlock *  taxa_block = reader.GetTaxaBlock(num_taxa_blocks-1);
            if (!taxa_block) {
                throw std::runtime_error("platypus::NclTreeReader::read_from_stream(): No taxon definitions were parsed (invalid file format?)");
            }
            NxsTreesBlock * trees_block = reader.GetTreesBlock(taxa_block, 0);
            if (!trees_block) {
                return 0;
            }
            unsigned int num_trees = trees_block->GetNumTrees();
            unsigned long tree_count = 0;
            for (unsigned int tree_idx = 0; tree_idx < num_trees; ++tree_idx) {
                auto & tree = get_new_tree_reference();
                const NxsFullTreeDescription & ftd = trees_block->GetFullTreeDescription(tree_idx);
                this->build_tree(tree, taxa_block, ftd, tree_count);
                ++tree_count;
                if (tree_limit > 0 && tree_count >= tree_limit) {
                    break;
                }
            }
            return tree_count;
        }

        void build_tree(TreeT& ttree,
                const NxsTaxaBlock * tb,
                const NxsFullTreeDescription & ftd,
                unsigned long tree_count=0) {
            this->set_tree_is_rooted(ttree, ftd.IsRooted());
            NxsSimpleTree ncl_tree(ftd, -1, -1.0);
            auto * root = ttree.head_node();
            decltype(root) node_parent = nullptr;
            decltype(root) new_node = nullptr;
            std::vector<const NxsSimpleNode *> ncl_nodes = ncl_tree.GetPreorderTraversal();
            std::map<const NxsSimpleNode *, decltype(root)> ncl_to_native;
            unsigned long num_leaf_nodes = 0;
            unsigned long num_internal_nodes = 1; // start at one to count root
            EdgeLengthT tree_length = 0.0;
            for (auto & ncl_node : ncl_nodes) {
                const NxsSimpleEdge & ncl_edge = ncl_node->GetEdgeToParentRef();
                const NxsSimpleNode * ncl_par = ncl_edge.GetParent();
                std::vector<NxsSimpleNode *> ncl_child_nodes = ncl_node->GetChildren();
                unsigned int nchildren = ncl_child_nodes.size();
                EdgeLengthT edge_len = static_cast<EdgeLengthT>(ncl_edge.GetDblEdgeLen());
                if (edge_len < 0) {
                    edge_len = 0.0;
                } else {
                    tree_length += edge_len;
                }
                std::string label;
                if (nchildren == 1) {
                    throw std::runtime_error("platypus::NclTreeReader::build_tree(): Tree source has node with only 1 child");
                } else if (nchildren == 0) {
                    unsigned int ncl_taxon_idx = ncl_node->GetTaxonIndex();
                    label = tb->GetTaxonLabel(ncl_taxon_idx).c_str();
                    // new_node = &this->tree_leaf_node_factory(ttree);
                    new_node = ttree.create_leaf_node();
                    ++num_leaf_nodes;
                } else {
                    label = NxsString::GetEscaped(ncl_node->GetName()).c_str();
                    if (!ncl_par) {
                        new_node = ttree.head_node();
                    } else {
                        // new_node = &this->tree_internal_node_factory(ttree);
                        new_node = ttree.create_internal_node();
                        ++num_internal_nodes;
                    }
                }
                this->set_node_value_label(new_node->value(), label);
                this->set_node_value_edge_length(new_node->value(), edge_len);
                ncl_to_native[ncl_node] = new_node;
                if (ncl_par) {
                    if (ncl_to_native.find(ncl_par) == ncl_to_native.end()) {
                        throw std::runtime_error("platypus::NclTreeReader::build_tree(): Parent node not visited in preorder traversal");
                    }
                    node_parent = ncl_to_native[ncl_par];
                    if (!node_parent) {
                        throw std::runtime_error("platypus::NclTreeReader::build_tree(): Null parent node");
                    }
                    node_parent->add_child(new_node);
                }
            }
            this->postprocess_tree(
                    ttree,
                    tree_count,
                    num_leaf_nodes,
                    num_internal_nodes,
                    tree_length);
        }

}; // NclTreeReader

} // namespace platypus

#endif
