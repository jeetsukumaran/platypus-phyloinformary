/**
 * @package     platypus-phyloinformary
 * @brief       Base class for readers.
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

#ifndef PLATYPUS_READER_HPP
#define PLATYPUS_READER_HPP

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <functional>
#include "treeproducer.hpp"

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// TreeReader

/**
 * @brief Encapsulates creation and population of TreeT objects from data
 * sources using the NEXUS Class Library (NCL).
 *
 * @tparam TreeT
 *   A class representing a specialization or a derived specialization of
 *   platypus::Tree.
 *
 * @detail This class serves as an adapter between the NCL data model and the
 * platypus data model. A function object that instantiates and returns new
 * TreeT objects must be provided; management of these objects' lifecycles is
 * the responsibility of the client code. Function objects or pointers to
 * member functions allow for the setting of tree's rooted state, as well as
 * node labels and edge lengths.
 *
 * Example of setting object state using function objects (in this case,
 * lambda expressions):
 *
 *  @code@
 *    struct NodeData {
 *        std::string label;
 *        double      edge_length;
 *    };
 *    typedef platypus::Tree<NodeData> TreeType;
 *    std::vector<TreeType> trees;
 *    auto tree_factory = [&trees] () -> TreeType& { trees.emplace_back(); return trees.back(); };
 *    auto is_rooted_f = [] (TreeType& tree, bool) {}; // no-op
 *    auto node_label_f = [] (NodeData& nd, const std::string& label) {nd.label = label;};
 *    auto node_edge_f = [] (NodeData& nd, double len) {nd.edge_length = len;};
 *    TreeReader<TreeType> tree_reader(tree_factory, is_rooted_f, node_label_f, node_edge_f);
 *    tree_reader.read_from_string("[&R](A:1,(B:1,(D:1,(E:1,F:1):1):1):1):1;", "newick");
 *  @endcode@
 *
 * Example of setting object state using pointer to member functions:
 *
 *  @code@
 *    class NodeData {
 *        public:
 *            void set_label(const std::string label) {
 *                this->label_ = label;
 *            }
 *            void set_edge_length(double edge_length) {
 *                this->edge_length_ = edge_length;
 *            }
 *        private:
 *            std::string label_;
 *            double      edge_length_;
 *    };
 *    typedef platypus::Tree<NodeData> TreeType;
 *    std::vector<TreeType> trees;
 *    auto tree_factory = [&trees] () -> TreeType& { trees.emplace_back(); return trees.back(); };
 *    TreeReader<TreeType> tree_reader(tree_factory, nullptr, &NodeData::set_label, &NodeData::set_edge_length);
 *    tree_reader.read_from_string("[&R](A:1,(B:1,(D:1,(E:1,F:1):1):1):1):1;", "newick");
 *  @endcode@
 *
 */
template <typename TreeT>
class TreeReader : public TreeProducer<TreeT> {

    public:
        typedef TreeProducer<TreeT>::tree_type                       tree_type;
        typedef TreeProducer<TreeT>::tree_node_type                  tree_node_type;
        typedef TreeProducer<TreeT>::tree_value_type                 tree_value_type;

    public:
        typedef TreeProducer<TreeT>::TreeFactoryType                 TreeFactoryType;
        typedef TreeProducer<TreeT>::TreeIsRootedFuncType            TreeIsRootedFuncType;
        typedef TreeProducer<TreeT>::SetNodeValueLabelFuncType       SetNodeValueLabelFuncType;
        typedef TreeProducer<TreeT>::SetNodeValueEdgeLengthFuncType  SetNodeValueEdgeLengthFuncType;

        // typdefs for pointers to member functions of various objects that can
        // be used to create functions listed above
        typedef void  (TreeT::*TreeIsRootedFuncPtrType)(bool);
        typedef void  (TreeT::value_type::*NodeValueLabelFuncPtrType)(const std::string&);
        typedef void  (TreeT::value_type::*NodeValueEdgeLengthFuncPtrType)(double);

    public:

        /**
         * @brief Constructs a TreeReader object that can parse data sources
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
        TreeReader(
                const TreeFactoryType & tree_factory,
                const TreeIsRootedFuncType & tree_is_rooted_func,
                const SetNodeValueLabelFuncType & node_value_label_func,
                const SetNodeValueEdgeLengthFuncType & node_value_edge_length_func)
            : TreeProducer<TreeT>(
                    tree_factory,
                    tree_is_rooted_func,
                    node_value_label_func,
                    node_value_edge_length_func) {
        }

        /**
         * @brief Overload of constructor that allows passing in pointers to
         * member functions of TreeT and TreeT::value_type to set the rooted
         * state (TreeT) and node labels and edge lengths (TreeT::value_type).
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
         *   A pointer to a member function of TreeT that takes a single
         *   boolean value as an argument and sets the rooted state of the
         *   tree accordingly.
         *
         * @param node_value_label_setter
         *   A pointer to a member function of TreeT::value_type that takes a
         *   single std::string value as an argument and sets the label of the
         *   node.
         *
         * @param node_value_edge_length_setter
         *   A pointer to a member function of TreeT::value_type that takes a
         *   single double value as an argument and sets the edge length of the
         *   edge subtending thenode.
         */
        TreeReader(
                const TreeFactoryType & tree_factory,
                TreeIsRootedFuncPtrType tree_is_rooted_setter=nullptr,
                NodeValueLabelFuncPtrType node_value_label_setter=nullptr,
                NodeValueEdgeLengthFuncPtrType node_value_edge_length_setter=nullptr) {
            this->set_tree_factory(tree_factory);
            this->set_tree_is_rooted_func(tree_is_rooted_setter);
            this->set_node_value_label_func(node_value_label_setter);
            this->set_node_value_edge_length_func(node_value_edge_length_setter);
        }

        void set_tree_is_rooted_func(TreeIsRootedFuncPtrType tree_is_rooted_func_ptr) {
            if (tree_is_rooted_func_ptr != nullptr) {
                this->set_tree_is_rooted_ = [tree_is_rooted_func_ptr] (tree_type& tree, bool is_rooted) { ((tree).*(tree_is_rooted_func_ptr))(is_rooted); };
            } else {
                this->clear_tree_is_rooted_func();
            }
        }

        void set_node_value_label_func(NodeValueLabelFuncPtrType node_value_label_func_ptr) {
            if (node_value_label_func_ptr != nullptr) {
                this->set_node_value_label_ = [node_value_label_func_ptr] (tree_value_type& nd, const std::string label) { ((nd).*(node_value_label_func_ptr))(label); };
            } else {
                this->clear_node_value_label_func();
            }
        }

        void set_node_value_edge_length_func(NodeValueEdgeLengthFuncPtrType node_value_edge_length_func_ptr) {
            if (node_value_edge_length_func_ptr != nullptr) {
                this->set_node_value_edge_length_ = [node_value_edge_length_func_ptr] (tree_value_type& nd, double len) { ((nd).*(node_value_edge_length_func_ptr))(len); };
            } else {
                this->clear_node_value_edge_length_func();
            }
        }

        virtual int read_from_stream(std::istream& src, const std::string& format) = 0;

        virtual int read_from_filepath(const std::string& filepath, const std::string& format="nexus") {
            std::ifstream f(filepath);
            if (!f.good()) {
                throw std::runtime_error("platypus::TreeReader::read_from_filepath(): Error opening file for input");
            }
            return this->read_from_stream(f, format);
        }

        virtual int read_from_string(const std::string& str, const std::string& format="nexus") {
            std::istringstream s(str);
            return this->read_from_stream(s, format);
        }

}; // TreeReader

} // namespace platypus

#endif
