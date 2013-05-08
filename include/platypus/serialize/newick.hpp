/**
 * @package     platypus-phyloinformary
 * @brief       Newick-format data writing.
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
 *
 */

#ifndef PLATYPUS_SERIALIZE_NEWICK_HPP
#define PLATYPUS_SERIALIZE_NEWICK_HPP

#include <stdlib.h>
#include <exception>
#include <string>
#include <memory>
#include "../base/base_writer.hpp"

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// NewickWriter

template <typename TreeT>
class NewickWriter : public BaseTreeWriter<TreeT> {

    public:
        typedef TreeT                          tree_type;
        typedef typename tree_type::node_type  tree_node_type;
        typedef typename tree_type::value_type tree_value_type;

    public:

        //////////////////////////////////////////////////////////////////////////////
        // Life-cycle

        NewickWriter()
            : suppress_rooting_(false)
              , suppress_internal_node_labels_(false)
              , suppress_edge_lengths_(false)
              , compact_spaces_(false) {
        }

        ~NewickWriter() {
        }

        //////////////////////////////////////////////////////////////////////////////
        // Main interface

        template <typename IterT>
        void write(std::ostream & out, IterT trees_begin, IterT trees_end) const {
            for (auto trees_iter = trees_begin; trees_iter != trees_end; ++trees_iter) {
                this->write(out, *trees_iter);
                out << "\n";
            }
        }

        //////////////////////////////////////////////////////////////////////////////
        // Support

        // workhorse
        void write(std::ostream & out, const tree_type & tree) const {
            if (this->tree_is_rooted_getter_ && !this->suppress_rooting_) {
                if (this->tree_is_rooted_getter_(tree)) {
                    out << "[&R]";
                } else {
                    out << "[&U]";
                }
                if (!this->compact_spaces_) {
                    out << " ";
                }
            }
            this->write_node(tree, tree.begin(), out);
            out << ";";
        }

        // support pointers
        void write(std::ostream & out, const tree_type * tree) const {
            this->write(out, *tree);
        }

        void write(std::ostream & out, const std::shared_ptr<tree_type> & tree) const {
            this->write(out, *tree);
        }

        //////////////////////////////////////////////////////////////////////////////
        // Customization

        void set_suppress_rooting(bool suppress) {
            this->suppress_rooting_ = suppress;
        }

        void set_suppress_internal_node_labels(bool suppress) {
            this->suppress_internal_node_labels_ = suppress;
        }

        void set_suppress_edge_lengths(bool suppress) {
            this->suppress_edge_lengths_ = suppress;
        }

        void set_compact_spaces(bool compact) {
            this->compact_spaces_ = compact;
        }

    protected:

        void write_node(const tree_type & tree,
                const typename tree_type::iterator & node_iter,
                std::ostream& out) const {
            bool is_leaf = node_iter.is_leaf();
            if (!is_leaf) {
                out << "(";
                int ch_count = 0;
                for (auto chi = tree.children_begin(node_iter);
                        chi != tree.children_end(node_iter);
                        ++chi, ++ch_count) {
                    if (ch_count > 0) {
                        out << ",";
                        if (!this->compact_spaces_) {
                            out << " ";
                        }
                    }
                    this->write_node(tree, chi, out);
                }
                out << ")";
            }
            if (this->node_value_label_getter_ && (is_leaf || !this->suppress_internal_node_labels_)) {
                out << this->node_value_label_getter_(*node_iter);
            }
            if (this->node_value_edge_length_getter_ && !this->suppress_edge_lengths_) {
                out << ":" << this->node_value_edge_length_getter_(*node_iter);
            }
        }

    protected:
        bool    suppress_rooting_;
        bool    suppress_internal_node_labels_;
        bool    suppress_edge_lengths_;
        bool    compact_spaces_;



}; // NewickWriter

} // platypus

#endif
