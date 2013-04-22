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
         * Default constructor.
         */
        TreeReader() {
        }

        virtual ~TreeReader() {
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
                throw std::runtime_error("platypus::TreeReader::read_from_filepath(): Error opening file for input");
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

}; // TreeReader

} // namespace platypus

#endif