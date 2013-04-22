/**
 * @package     platypus-phyloinformary
 * @brief       Newick-format tree reader.
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

#include <iomanip>
#include "tests.hpp"

//--------------------------------------------------------------------------------

#include <stdlib.h>
#include <exception>
#include <string>
#include <platypus/datareader.hpp>
#include <platypus/tokenizer.hpp>

namespace platypus {

class PlatypusException : public std::exception {
    public:
        PlatypusException(const std::string& message)
            : message_(message) {
        }
        PlatypusException(const char * message)
            : message_(message) {
        }
        virtual ~PlatypusException() throw() { }
        virtual const char * what() const throw() {
            return this->message_.c_str();
        }

    private:
        std::string message_;
};

/**
 * Parses NEWICK tree data sources and instantiates corresponding tree objects.
 * @tparam TreeT
 */
template <typename TreeT>
class NewickReader : public BaseTreeReader<TreeT> {

    public:
        typedef TreeT                          tree_type;
        typedef typename tree_type::node_type  tree_node_type;
        typedef typename tree_type::value_type tree_value_type;

    public:

        /**
         * Exception thrown when encountering a non-recognized NEWICK token.
         */
        class NewickReaderInvalidTokenException : public PlatypusException {
            public:
                NewickReaderInvalidTokenException(const std::string& message)
                    : PlatypusException(message) { }
        };

        /**
         * Exception thrown when encountering a non-recognized NEWICK token.
         */
        class NewickReaderMalformedStatement : public PlatypusException {
            public:
                NewickReaderMalformedStatement(const std::string& message)
                    : PlatypusException(message) { }
        };

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
            while (src_iter != this->tokenizer_.end()) {
                auto & tree = this->create_new_tree();
                this->parse_tree_from_stream(tree, src_iter);
            }
            return 0;
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
         * following the end of the node, i.e. the closing parenthesis of the
         * tree statement or the comma following a leaf label.
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

} // namespace platypus

//--------------------------------------------------------------------------------

struct NodeData {
    std::string label;
    double      edge_length;
    NodeData & operator=(const NodeData & nd) {
        this->label = nd.label;
        this->edge_length = nd.edge_length;
        return *this;
    }
};

int main () {

    std::string tree_string =
        "[&R] ((((T0:0.083, T5:0.083):0.12, T1:0.2):0.64, T4:0.84):0.32, (T2:0.042, T3:0.042):1.1):0;"
        "[&R] ((T1:1, T0:1):2, ((T4:0.14, T2:0.14):0.53, (T5:0.1, T3:0.1):0.57):2.4):0;"
        "[&R] ((T3:0.75, (T2:0.32, ((T0:0.067, T5:0.067):0.05, T1:0.12):0.21):0.43):0.044, T4:0.8):0;"
        "[&R] (((T1:0.3, T5:0.3):0.079, (T3:0.25, (T0:0.076, T2:0.076):0.17):0.13):0.81, T4:1.2):0;"
        "[&R] ((T2:0.075, T5:0.075):0.45, ((T3:0.31, (T4:0.078, T0:0.078):0.23):0.058, T1:0.36):0.16):0;"
        ;

    typedef platypus::Tree<NodeData> TreeType;
    std::vector<TreeType> trees;
    auto tree_factory = [&trees] () -> TreeType& { trees.emplace_back(); return trees.back(); };
    auto is_rooted_f = [] (TreeType& tree, bool) {}; // no-op
    auto node_label_f = [] (NodeData& nd, const std::string& label) {nd.label = label;};
    auto node_edge_f = [] (NodeData& nd, double len) {nd.edge_length = len;};
    platypus::NewickReader<TreeType> tree_reader;
    tree_reader.set_tree_factory(tree_factory);
    tree_reader.set_tree_is_rooted_setter(is_rooted_f);
    tree_reader.set_node_label_setter(node_label_f);
    tree_reader.set_edge_length_setter(node_edge_f);

    tree_reader.read_from_string(tree_string, "newick");
    // tree_reader.read_from_string("(a b); ", "newick"); // test case: missing comma
    // tree_reader.read_from_string("(a, b, ,,); ", "newick"); // test case: extra commas (should be treated as blank nodes); TODO: deal with this case!!
    // tree_reader.read_from_string("(a, (b, c)) ", "newick"); // test case: missing semi-colon;

    const std::function<void (typename TreeType::value_type &, std::ostream &)> write_node_f(
            [] (typename TreeType::value_type & nv, std::ostream & out) {
                if (!nv.label.empty()) {
                    out << nv.label;
                }
                out << ":" << std::setprecision(4) << nv.edge_length;
            });
    for (auto & tree : trees) {
        write_newick(tree, std::cout, write_node_f);
    }
}

