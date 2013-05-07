/**
 * @package     platypus-phyloinformary
 * @brief       Base class for classes that produce trees, alignments, etc.
 *              from reading formatted-data sources.
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

#ifndef PLATYPUS_BASE_READER_BASE_HPP
#define PLATYPUS_BASE_READER_BASE_HPP

#include <stdexcept>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "base_producer.hpp"

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// ReaderException

class ReaderException : public ProducerException {
    public:
        ReaderException(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : ProducerException(filename, line_num, message) { }
};

////////////////////////////////////////////////////////////////////////////////
// BaseTreeReader

/**
 * @brief Encapsulates creation and population of TreeT objects from data
 * sources.
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
class BaseTreeReader : public BaseTreeProducer<TreeT> {

    public:
        typedef typename BaseTreeProducer<TreeT>::tree_type             tree_type;
        typedef typename BaseTreeProducer<TreeT>::tree_node_type        tree_node_type;
        typedef typename BaseTreeProducer<TreeT>::tree_value_type       tree_value_type;

    public:
        typedef typename BaseTreeProducer<TreeT>::tree_factory_fntype                    tree_factory_fntype;
        typedef typename BaseTreeProducer<TreeT>::tree_is_rooted_setter_fntype           tree_is_rooted_setter_fntype;
        typedef typename BaseTreeProducer<TreeT>::node_value_label_setter_fntype         node_value_label_setter_fntype;
        typedef typename BaseTreeProducer<TreeT>::node_value_edge_length_setter_fntype   node_value_edge_length_setter_fntype;
        typedef typename BaseTreeProducer<TreeT>::tree_stats_numeric_setter_fntype       tree_stats_numeric_setter_fntype;
        typedef typename BaseTreeProducer<TreeT>::tree_stats_count_setter_fntype         tree_stats_count_setter_fntype;

    public:

        //////////////////////////////////////////////////////////////////////////////
        // Life-cycle

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
            : BaseTreeProducer<TreeT>(tree_factory) {
        }

        /**
         * Default constructor.
         */
        BaseTreeReader() {
        }

        virtual ~BaseTreeReader() {
        }

        //////////////////////////////////////////////////////////////////////////////
        // Reading interface

        // To be implementad by derived classes.
        virtual int parse_stream(std::istream& src, const std::string& format="") = 0;

        virtual int read_from_filepath(const std::string& filepath, const std::string& format="") {
            std::ifstream f(filepath);
            if (!f.good()) {
                throw ReaderException(__FILE__, __LINE__, "platypus::BaseTreeReader::read_from_filepath(): Error opening file for input");
            }
            return this->parse_stream(f, format);
        }

        virtual int read_from_string(const std::string& str, const std::string& format="") {
            std::istringstream s(str);
            return this->parse_stream(s, format);
        }

        virtual int read_from_stream(std::istream& src, const std::string& format="") {
            return this->parse_stream(src, format);
        }

}; // BaseTreeReader

} // namespace platypus

#endif
