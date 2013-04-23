/**
 * @package     platypus-phyloinformary
 * @brief       Newick-format data input/output.
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

#ifndef PLATYPUS_NEWICK_HPP
#define PLATYPUS_NEWICK_HPP

#include <stdlib.h>
#include <exception>
#include <string>
#include "tokenizer.hpp"
#include "treebuilder.hpp"

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// NewickReader Exceptions

/**
 * Exception thrown when encountering a non-recognized NEWICK token.
 */
class NewickReaderException : public TreeReaderException {
public:
    NewickReaderException(const std::string& message)
        : TreeReaderException(message) { }
};

/**
 * Exception thrown when encountering a non-recognized NEWICK token.
 */
class NewickReaderInvalidTokenException : public NewickReaderException {
    public:
        NewickReaderInvalidTokenException(const std::string& message)
            : NewickReaderException(message) { }
};

/**
 * Exception thrown when encountering a non-recognized NEWICK token.
 */
class NewickReaderMalformedStatement : public NewickReaderException {
    public:
        NewickReaderMalformedStatement(const std::string& message)
            : NewickReaderException(message) { }
};

////////////////////////////////////////////////////////////////////////////////
// NewickReader

/**
 * Parses NEWICK tree data sources and instantiates corresponding tree objects.
 */
template <typename TreeT>
class NewickReader : public BaseTreeReader<TreeT> {

    public:
        typedef TreeT                          tree_type;
        typedef typename tree_type::node_type  tree_node_type;
        typedef typename tree_type::value_type tree_value_type;

    public:

        NewickReader(
                const typename BaseTreeReader<TreeT>::tree_factory_fntype & tree_factory,
                const typename BaseTreeReader<TreeT>::tree_is_rooted_setter_fntype & tree_is_rooted_func,
                const typename BaseTreeReader<TreeT>::node_value_label_setter_fntype & node_value_label_func,
                const typename BaseTreeReader<TreeT>::node_value_edge_length_setter_fntype & node_value_edge_length_func)
            : BaseTreeReader<TreeT>(tree_factory,
                    tree_is_rooted_func,
                    node_value_label_func,
                    node_value_edge_length_func) {
        }

        NewickReader() {
        }

        int parse_from_stream(std::istream& src, const std::string&
                format="newick") override {
            if (format != "newick") {
                throw std::logic_error("platypus::NewickReader only supports 'newick' formatted sources");
            }
            NexusTokenizer::iterator src_iter = this->tokenizer_.begin(src);
            int tree_count = 0;
            while (src_iter != this->tokenizer_.end()) {
                auto & tree = this->create_new_tree();
                this->parse_tree_from_stream(tree, src_iter);
                ++tree_count;
            }
            return tree_count;
        }

        /**
         * Parses a single tree statement from a token stream and constructs a
         * corresponding TreeT object. Expects the current token to be the
         * first parenthesis of a tree statement. When complete, the current
         * token will be the token immediately following the semi-colon, if
         * any.
         *
         * @param tree
         *   A new TreeT object.
         * @param iter
         *   An iterator over the token stream. Expects the current token to
         *   be the first parenthesis of a tree statement.
         */
        tree_type & parse_tree_from_stream(TreeT & tree,
                NexusTokenizer::iterator & src_iter) {
            if (*src_iter != "(") {
                throw NewickReaderInvalidTokenException(*src_iter);
            }
            this->parse_node_from_stream(tree, tree.head_node(), src_iter);
            return tree;
        }

        /**
         * Assuming that the iterator is currently sitting on a parenthesis
         * that opens a node with children or the label of a leaf node, this
         * will populate the node ``node`` appropriately (label, edge length,
         * comments, metadata etc.) and recursively parse and add the node's
         * children. When complete, the token will be the token immediately
         * following the end of the node or tree statement if this is the root
         * node, i.e. the token following the closing parenthesis of the node
         * or the comma following a leaf label or the token following the
         * semi-colon terminating a tree statement.
         */
        tree_node_type * parse_node_from_stream(
                tree_type & tree,
                tree_node_type * current_node,
                NexusTokenizer::iterator & src_iter) {
            if (*src_iter == "(") {
                // begin processing of child nodes
                if (src_iter.eof()) {
                    throw NewickReaderMalformedStatement("platypus::NewickReader: premature end of stream");
                }
                src_iter.require_next();
                while (true) {
                    if (src_iter.eof()) {
                        throw NewickReaderMalformedStatement("platypus::NewickReader: premature end of stream");
                    }
                    if (*src_iter == ",") {
                        // next child
                        src_iter.require_next();
                        while (*src_iter == ",") {
                            // empty name (as allowed by NEWICK standard)
                            auto new_node = tree.create_leaf_node();
                            current_node->add_child(new_node);
                            src_iter.require_next();
                        }
                    } else if (*src_iter == ")") {
                        // end of child nodes
                        src_iter.require_next();
                        break;
                    } else {
                        // assume child nodes: a leaf node (if a label) or
                        // internal (if a parenthesis)
                        if (*src_iter == "(") {
                            // internal nodes
                            auto new_node = tree.create_internal_node();
                            this->parse_node_from_stream(tree, new_node, src_iter);
                            current_node->add_child(new_node);
                        } else {
                            // assume a label token, i.e. a leaf node
                            auto new_node = tree.create_leaf_node();
                            this->parse_node_from_stream(tree, new_node, src_iter);
                            current_node->add_child(new_node);
                        }
                    }
                }
            }
            if (src_iter.eof()) {
                throw NewickReaderMalformedStatement("platypus::NewickReader: premature end of stream");
            }
            bool label_parsed = false;
            while (true) {
                if (*src_iter == ":") {
                    ++src_iter;
                    double edge_len = std::atof(src_iter->c_str());
                    this->set_node_value_edge_length(current_node->value(), edge_len);
                    src_iter.require_next();
                } else if (*src_iter == ")") {
                    // closing of parent token
                    return current_node;
                } else if (*src_iter == ";") {
                    // end of tree statement
                    ++src_iter;
                    break;
                } else if (*src_iter == ",") {
                    // end of this node
                    return current_node;
                } else if (*src_iter == "(") {
                    // start of another node or tree without finishing this
                    // node
                        throw NewickReaderMalformedStatement("platypus::NewickReader: malformed tree statement");
                } else {
                    // label
                    if (label_parsed) {
                        throw NewickReaderMalformedStatement("platypus::NewickReader: Expecting ':', ')', ',' or ';' after reading label");
                    } else {
                        this->set_node_value_label(current_node->value(), *src_iter);
                        label_parsed = true;
                        src_iter.require_next();
                    }
                }
            }
            return current_node;
        }


    private:
        NexusTokenizer      tokenizer_;

}; // NewickTreeReader


} // platypus

#endif
