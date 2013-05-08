/**
 * @package     platypus-phyloinformary
 * @brief       Base class for classes that produce trees, alignments
 *              and other objects: parsers, simulators, etc.
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

#ifndef PLATYPUS_BASE_PRODUCER_BASE_HPP
#define PLATYPUS_BASE_PRODUCER_BASE_HPP

#include <stdexcept>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <functional>
#include "exception.hpp"

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// ProducerException

class ProducerException : public PlatypusException {
    public:
        ProducerException(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : PlatypusException(filename, line_num, message) { }
};

////////////////////////////////////////////////////////////////////////////////
// BaseTreeProducer (base class for all tree-producing classes)

template <typename TreeT, typename EdgeLengthT=double>
class BaseTreeProducer {

    public:

        // typdefs for data
        typedef TreeT                            tree_type;
        typedef typename tree_type::node_type    tree_node_type;
        typedef typename tree_type::value_type   tree_value_type;

        // typedefs for functions used in construction
        typedef std::function<void (tree_type &, bool)>                                                        tree_is_rooted_setter_fntype;
        typedef std::function<void (tree_value_type &, const std::string &)>                                   node_value_label_setter_fntype;
        typedef std::function<void (tree_value_type &, EdgeLengthT)>                                           node_value_edge_length_setter_fntype;
        typedef std::function<void (tree_type &, unsigned long, unsigned long, unsigned long, EdgeLengthT)>    tree_postprocess_fntype;


    public:

        BaseTreeProducer() { }

        virtual ~BaseTreeProducer() { }

        // Setting/binding of functions


        /**
         * Binds the tree rooting state setter function.
         *
         * @param tree_is_rooted_func
         *   A function that takes a reference to a TreeT object and a boolean
         *   value representing whether or not the tree is (or should be)
         *   rooted, and sets the state of the TreeT object accordingly.
         */
        virtual void set_tree_is_rooted_setter(const tree_is_rooted_setter_fntype & tree_is_rooted_func) {
            this->tree_is_rooted_setter_ = tree_is_rooted_func;
        }
        virtual void clear_tree_is_rooted_setter() {
            this->tree_is_rooted_setter_ = [] (tree_type&, bool) { };
        }

        /**
         * Binds the node label setter function.
         *
         * @param node_value_label_func
         *   A function that takes a reference to a TreeT::value_type object
         *   (representing the data stored at particular node in the tree) and
         *   a string value representing the label for that node and sets the
         *   object's state accordingly.
         */
        virtual void set_node_label_setter(const node_value_label_setter_fntype & node_value_label_func) {
            this->node_value_label_setter_ = node_value_label_func;
        }
        virtual void clear_node_label_setter() {
            this->node_value_label_setter_ = [] (tree_value_type&, const std::string&) { };
        }

        /**
         * Binds the edge length setter function.
         *
         * @param node_value_edge_length_func
         *   A function that takes a reference to a TreeT::value_type object
         *   (representing the data stored at a particular node in the tree)
         *   and a EdgeLnegthT (by default, a double) value representing the
         *   weight or length of the edge subtending the node, and sets the
         *   object's state accordingly.
         */
        virtual void set_edge_length_setter(const node_value_edge_length_setter_fntype & node_value_edge_length_func) {
            this->node_value_edge_length_setter_ = node_value_edge_length_func;
        }
        virtual void clear_edge_length_setter() {
            this->node_value_edge_length_setter_ = [] (tree_value_type&, EdgeLengthT) { };
        }

        /**
         * Binds the tree post-processing function.
         *
         * @param tree_postprocess_fn
         *   A function that takes, in order, the following arguments:
         *      - an TreeT object reference, representing the tree produced
         *      - an unsigned long value, representing the 0-based index of the tree
         *        produced (first tree produced has index of 0)
         *      - an unsigned long value, representing the number of tips or
         *        leaf nodes in the tree
         *      - an unsigned long value, representing the number of internal nodes of the tree
         *      - an EdgeLengthT argument, representing the total tree length,
         *        i.e. the sum of edge lengths on the tree
         */
        virtual void set_tree_postprocess_fn(const tree_postprocess_fntype & tree_postprocess_fn) {
            this->tree_postprocess_fn_ = tree_postprocess_fn;
        }
        virtual void clear_tree_postprocess_fn() {
            this->tree_postprocess_fn_ = [] (tree_type &, unsigned long, unsigned long, unsigned long, EdgeLengthT) { };
        }

    protected:

        void set_tree_is_rooted(tree_type & tree, bool is_rooted) {
            if (this->tree_is_rooted_setter_) {
                this->tree_is_rooted_setter_(tree, is_rooted);
            }
        }
        void set_node_value_label(tree_value_type & nv, const std::string & label) {
            if (this->node_value_label_setter_) {
                this->node_value_label_setter_(nv, label);
            }
        }
        void set_node_value_edge_length(tree_value_type & nv, EdgeLengthT length) {
            if (this->node_value_edge_length_setter_) {
                this->node_value_edge_length_setter_(nv, length);
            }
        }
        void postprocess_tree(tree_type & tree, unsigned long idx, unsigned long tips, unsigned long internals, EdgeLengthT length) {
            if (this->tree_postprocess_fn_) {
                this->tree_postprocess_fn_(tree, idx, tips, internals, length);
            }
        }

    protected:
        tree_is_rooted_setter_fntype                tree_is_rooted_setter_;
        node_value_label_setter_fntype              node_value_label_setter_;
        node_value_edge_length_setter_fntype        node_value_edge_length_setter_;
        tree_postprocess_fntype                     tree_postprocess_fn_;

}; // BaseTreeProducer

} // namespace platypus

#endif
