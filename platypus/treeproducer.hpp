/**
 * @package     platypus-phyloinformary
 * @brief       Base class for classes that producer trees: e.g. simulators,
 *              parsers etc.
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

#ifndef PLATYPUS_TREEPRODUCER_HPP
#define PLATYPUS_TREEPRODUCER_HPP

#include <functional>

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// TreeProducer (base class)

template <typename TreeT>
class TreeProducer {

    public:

        // typdefs for data
        typedef TreeT                           tree_type;
        typedef typename tree_type::node_type    tree_node_type;
        typedef typename tree_type::value_type   tree_value_type;

        // typedefs for functions used in construction
        typedef std::function<tree_type & ()>                                  tree_factory_fntype;
        typedef std::function<void (tree_type &, bool)>                        tree_is_rooted_setter_fntype;
        typedef std::function<void (tree_value_type &, const std::string &)>   node_value_label_setter_fntype;
        typedef std::function<void (tree_value_type &, double)>                node_value_edge_length_setter_fntype;

    public:

        /**
         * Sets the service functions of the TreeProducer.
         *
         * @param tree_factory
         *   A Function object that takes no arguments and returns a reference
         *   to a new TreeT object. This function should take responsibility
         *   for allocating memory, constructing, and initializing the TreeT
         *   object. In addition, the function should also take responsibility
         *   for storage ofthe object. Client code is responsible for the
         *   management (including disposal) of the object.
         *
         * @param tree_is_rooted_func
         *   A function object that takes a TreeT object and a boolean value
         *   representing whether or not the tree is rooted (true == rooted)
         *   as arguments and sets the rooted state of the tree accordingly.
         *
         * @param node_value_label_func
         *   A function object that takes a reference to TreeT::value_type and
         *   a std::string value as arguments and sets the label of the
         *   node accordingly.
         *
         * @param node_value_edge_length_func
         *   A function object that takes a reference to TreeT::value_type and
         *   a double value as arguments and sets the edge length of the node
         *   accordingly.
         */
        TreeProducer(
                const tree_factory_fntype & tree_factory,
                const tree_is_rooted_setter_fntype & tree_is_rooted_func,
                const node_value_label_setter_fntype & node_value_label_func,
                const node_value_edge_length_setter_fntype & node_value_edge_length_func) {
            this->set_tree_factory(tree_factory);
            this->set_tree_is_rooted_func(tree_is_rooted_func);
            this->set_node_value_label_func(node_value_label_func);
            this->set_node_value_edge_length_func(node_value_edge_length_func);
        }

        TreeProducer() { }

        virtual ~TreeProducer() { }


        // Setting/binding of functions
        /**
         * @brief Sets the function object that will be called to allocate,
         * construct and * return a new TreeT object.
         *
         * @param tree_factory
         *   A Function object that takes no arguments and returns a reference
         *   to a new TreeT object. This function should take responsibility
         *   for allocating memory, constructing, and initializing the TreeT
         *   object. In addition, the function should also take responsibility
         *   for storage ofthe object. Client code is responsible for the
         *   management (including disposal) of the object.
         *
         */
        virtual void set_tree_factory(const tree_factory_fntype & tree_factory) {
            this->tree_factory_ = tree_factory;
        }
        virtual void set_tree_is_rooted_func(const tree_is_rooted_setter_fntype & tree_is_rooted_func) {
            this->set_tree_is_rooted_ = tree_is_rooted_func;
        }
        virtual void clear_tree_is_rooted_func() {
            this->set_tree_is_rooted_ = [] (tree_type&, bool) { };
        }

        virtual void set_node_value_label_func(const node_value_label_setter_fntype & node_value_label_func) {
            this->set_node_value_label_ = node_value_label_func;
        }
        virtual void clear_node_value_label_func() {
            this->set_node_value_label_ = [] (tree_value_type&, const std::string&) { };
        }

        virtual void set_node_value_edge_length_func(const node_value_edge_length_setter_fntype & node_value_edge_length_func) {
            this->set_node_value_edge_length_ = node_value_edge_length_func;
        }
        virtual void clear_node_value_edge_length_func() {
            this->set_node_value_edge_length_ = [] (tree_value_type&, double) { };
        }

        // Use of functions

        virtual void set_tree_is_rooted(tree_type & tree, bool is_rooted) {
            if (this->set_tree_is_rooted_fn_) {
                this->set_tree_is_rooted_fn_(tree);
            }
        }
        virtual void set_node_value_label(tree_type & tree, bool is_rooted) {
            if (this->set_node_value_label_fn_) {
                this->set_node_value_label_fn_(tree);
            }
        }
        virtual void set_node_value_edge_length(tree_type & tree, bool is_rooted) {
            if (this->set_node_value_edge_length_fn_) {
                this->set_node_value_edge_length_fn_(tree);
            }
        }

    protected:
        tree_factory_fntype                         tree_factory_;
        tree_is_rooted_setter_fntype                set_tree_is_rooted_fn_;
        node_value_label_setter_fntype              set_node_value_label_fn_;
        node_value_edge_length_setter_fntype        set_node_value_edge_length_fn_;

}; // TreeProducer

} // namespace platypus

#endif
