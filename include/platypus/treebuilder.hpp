/**
 * @package     platypus-phyloinformary
 * @brief       Base class for classes that produce Platypus objects: parsers,
 *              simulators, etc.
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

#ifndef PLATYPUS_TREEBUILDER_HPP
#define PLATYPUS_TREEBUILDER_HPP

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
// TreeProducerException

class TreeProducerException : public PlatypusException {
public:
    TreeProducerException(const std::string& message)
        : PlatypusException(message) { }
};

////////////////////////////////////////////////////////////////////////////////
// TreeProducer (base class)

template <typename TreeT>
class TreeProducer {

    public:

        // typdefs for data
        typedef TreeT                            tree_type;
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
            this->set_tree_is_rooted_setter(tree_is_rooted_func);
            this->set_node_label_setter(node_value_label_func);
            this->set_edge_length_setter(node_value_edge_length_func);
        }

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
         */
        TreeProducer(const tree_factory_fntype & tree_factory) {
            this->set_tree_factory(tree_factory);
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
            this->tree_factory_fn_ = tree_factory;
        }
        virtual void set_tree_is_rooted_setter(const tree_is_rooted_setter_fntype & tree_is_rooted_func) {
            this->tree_is_rooted_setter_ = tree_is_rooted_func;
        }
        virtual void clear_tree_is_rooted_setter() {
            this->tree_is_rooted_setter_ = [] (tree_type&, bool) { };
        }

        virtual void set_node_label_setter(const node_value_label_setter_fntype & node_value_label_func) {
            this->node_value_label_setter_ = node_value_label_func;
        }
        virtual void clear_node_label_setter() {
            this->node_value_label_setter_ = [] (tree_value_type&, const std::string&) { };
        }

        virtual void set_edge_length_setter(const node_value_edge_length_setter_fntype & node_value_edge_length_func) {
            this->node_value_edge_label_setter_ = node_value_edge_length_func;
        }
        virtual void clear_edge_length_setter() {
            this->node_value_edge_label_setter_ = [] (tree_value_type&, double) { };
        }

        // Use of functions
        virtual tree_type & create_new_tree() {
            if (!this->tree_factory_fn_) {
                throw std::runtime_error("platypus::TreeProducer::create_new_tree(): Unbound tree factory");
            }
            return this->tree_factory_fn_();
        }
        virtual void set_tree_is_rooted(tree_type & tree, bool is_rooted) {
            if (this->tree_is_rooted_setter_) {
                this->tree_is_rooted_setter_(tree, is_rooted);
            }
        }
        virtual void set_node_value_label(tree_value_type & nv, const std::string & label) {
            if (this->node_value_label_setter_) {
                this->node_value_label_setter_(nv, label);
            }
        }
        virtual void set_node_value_edge_length(tree_value_type & nv, double length) {
            if (this->node_value_edge_label_setter_) {
                this->node_value_edge_label_setter_(nv, length);
            }
        }

    protected:
        tree_factory_fntype                         tree_factory_fn_;
        tree_is_rooted_setter_fntype                tree_is_rooted_setter_;
        node_value_label_setter_fntype              node_value_label_setter_;
        node_value_edge_length_setter_fntype        node_value_edge_label_setter_;

}; // TreeProducer

////////////////////////////////////////////////////////////////////////////////
// TreeReaderException

class TreeReaderException : public PlatypusException {
public:
    TreeReaderException(const std::string& message)
        : PlatypusException(message) { }
};

////////////////////////////////////////////////////////////////////////////////
// BaseTreeReader

/**
 * @brief Encapsulates creation and population of TreeT objects from data
 * sources using the NEXUS Class Library (NCL).
 *
 * @tparam TreeT
 *   A class representing a specialization or a derived specialization of
 *   platypus::Tree.
 *
 * @detail
 *   This class serves as a base class for specializations to deal with data
 *   formats of different kinds. A function object that instantiates and
 *   returns new TreeT objects must be provided; management of these objects'
 *   lifecycles is the responsibility of the client code. Function objects
 *   allow for the setting of tree's rooted state, as well as node labels and
 *   edge lengths.
 *
 * Example of setting object state using function objects (in this case,
 * lambda expressions):
 *
 *  @code@
 *    struct NodeData {
 *        std::string label;
 *        double      edge_length;
 *        NodeData & operator=(const NodeData & nd) {
 *            this->label = nd.label;
 *            this->edge_length = nd.edge_length;
 *            return *this;
 *        }
 *    };
 *    typedef platypus::Tree<NodeData> TreeType;
 *    std::vector<TreeType> trees;
 *    auto tree_factory = [&trees] () -> TreeType& { trees.emplace_back(); return trees.back(); };
 *    platypus::SomeTreeReader<TreeType> tree_reader(tree_factory);
 *    // or:
 *    // platypus::SomeTreeReader<TreeType> tree_reader;
 *    // tree_reader.set_tree_factory(tree_factory);
 *    auto is_rooted_f = [] (TreeType& tree, bool) {}; // no-op
 *    tree_reader.set_tree_is_rooted_setter(is_rooted_f);
 *    auto node_label_f = [] (NodeData& nd, const std::string& label) {nd.label = label;};
 *    tree_reader.set_node_label_setter(node_label_f);
 *    auto node_edge_f = [] (NodeData& nd, double len) {nd.edge_length = len;};
 *    tree_reader.set_edge_length_setter(node_edge_f);
 *    tree_reader.read_from_string("[&R](A:1,(B:1,(D:1,(E:1,F:1):1):1):1):1;", "newick");
 *  @endcode@
 *
 */
template <typename TreeT>
class BaseTreeReader : public TreeProducer<TreeT> {

    public:
        typedef typename TreeProducer<TreeT>::tree_type             tree_type;
        typedef typename TreeProducer<TreeT>::tree_node_type        tree_node_type;
        typedef typename TreeProducer<TreeT>::tree_value_type       tree_value_type;

    public:
        typedef typename TreeProducer<TreeT>::tree_factory_fntype                    tree_factory_fntype;
        typedef typename TreeProducer<TreeT>::tree_is_rooted_setter_fntype           tree_is_rooted_setter_fntype;
        typedef typename TreeProducer<TreeT>::node_value_label_setter_fntype         node_value_label_setter_fntype;
        typedef typename TreeProducer<TreeT>::node_value_edge_length_setter_fntype   node_value_edge_length_setter_fntype;

    public:

        /**
         * @brief Constructs a BaseTreeReader object that can parse data sources
         * and instantiate corresponding TreeT objects, with labels and edge
         * lengths set by function objects passed as arguments.
         *
         * @param tree_factory
         *   A Function object that takes no arguments and returns a reference
         *   to a new TreeT object. This function should take responsibility
         *   for allocating memory, constructing, and initializing the TreeT
         *   object. In addition, the function should also take responsibility
         *   for storage ofthe object. Client code is responsible for the
         *   management (including disposal) of the object.
         *
         * @param tree_is_rooted_setter
         *   A function object that takes a TreeT object and a boolean value
         *   representing whether or not the tree is rooted (true == rooted)
         *   as arguments and sets the rooted state of the tree accordingly.
         *
         * @param node_value_label_setter
         *   A function object that takes a reference to TreeT::value_type and
         *   a std::string value as arguments and sets the label of the
         *   node accordingly.
         *
         * @param node_value_label_setter
         *   A function object that takes a reference to TreeT::value_type and
         *   a double value as arguments and sets the edge length of the node
         *   accordingly.
         */
        BaseTreeReader(
                const tree_factory_fntype & tree_factory,
                const tree_is_rooted_setter_fntype & tree_is_rooted_func,
                const node_value_label_setter_fntype & node_value_label_func,
                const node_value_edge_length_setter_fntype & node_value_edge_length_func)
            : TreeProducer<TreeT>(
                    tree_factory,
                    tree_is_rooted_func,
                    node_value_label_func,
                    node_value_edge_length_func) {
        }

        /**
         * @brief Constructs a BaseTreeReader object that can parse data sources
         * and instantiate corresponding TreeT objects.
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
        BaseTreeReader(const tree_factory_fntype & tree_factory)
            : TreeProducer<TreeT>(tree_factory) {
        }

        /**
         * Default constructor.
         */
        BaseTreeReader() {
        }

        virtual ~BaseTreeReader() {
        }

        /**
         * To be implementad by derived classes.
         *
         * @param src
         * @param format
         *
         * @return
         */
        virtual int parse_from_stream(std::istream& src, const std::string& format) = 0;

        virtual int read_from_filepath(const std::string& filepath, const std::string& format="nexus") {
            std::ifstream f(filepath);
            if (!f.good()) {
                throw TreeReaderException("platypus::BaseTreeReader::read_from_filepath(): Error opening file for input");
            }
            return this->parse_from_stream(f, format);
        }

        virtual int read_from_string(const std::string& str, const std::string& format="nexus") {
            std::istringstream s(str);
            return this->parse_from_stream(s, format);
        }

        virtual int read_from_stream(std::istream& src, const std::string& format) {
            return this->parse_from_stream(src, format);
        }

}; // BaseTreeReader

} // namespace platypus

#endif
