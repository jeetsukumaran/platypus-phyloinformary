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
#include <vector>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
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
 *    auto get_new_tree_reference = [&trees] () -> TreeType& { trees.emplace_back(); return trees.back(); };
 *    platypus::SomeTreeReader<TreeType> tree_reader(get_new_tree_reference);
 *    // or:
 *    // platypus::SomeTreeReader<TreeType> tree_reader;
 *    // tree_reader.set_get_new_tree_reference(get_new_tree_reference);
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
template <typename TreeT, typename EdgeLengthT=double>
class BaseTreeReader : public BaseTreeProducer<TreeT, EdgeLengthT> {

    public:
        typedef typename BaseTreeProducer<TreeT, EdgeLengthT>::tree_type             tree_type;
        typedef typename BaseTreeProducer<TreeT, EdgeLengthT>::tree_node_type        tree_node_type;
        typedef typename BaseTreeProducer<TreeT, EdgeLengthT>::tree_value_type       tree_value_type;

    public:
        typedef typename BaseTreeProducer<TreeT, EdgeLengthT>::tree_is_rooted_setter_fntype            tree_is_rooted_setter_fntype;
        typedef typename BaseTreeProducer<TreeT, EdgeLengthT>::node_value_label_setter_fntype          node_value_label_setter_fntype;
        typedef typename BaseTreeProducer<TreeT, EdgeLengthT>::node_value_edge_length_setter_fntype    node_value_edge_length_setter_fntype;
        typedef typename BaseTreeProducer<TreeT, EdgeLengthT>::tree_postprocess_fntype                 tree_postprocess_fntype;

    public:

        //////////////////////////////////////////////////////////////////////////////
        // Life-cycle

        BaseTreeReader() {
        }

        virtual ~BaseTreeReader() {
        }

        //////////////////////////////////////////////////////////////////////////////
        // Reading interface

        /**
         * Reads the stream `src`, creating tree objects using `get_new_tree_reference`
         * and populating them based on the data in the stream.
         *
         * @param src
         *   Input stream which contains a representation of the tree(s).
         *
         * @param get_new_tree_reference
         *   A Function object that takes no arguments and returns a
         *   *reference* to a new TreeT object. This function will be called
         *   multiple times, once for each tree produced by the producer. This
         *   function should take responsibility for allocating memory,
         *   constructing, and initializing the TreeT object. In addition, the
         *   function should also take responsibility for storage of the
         *   object.  Client code is responsible for the lifecycle management
         *   (including disposal) of the object.
         *
         *   Examples of factory functions:
         *
         *      // Container (vector) of Tree objects
         *      std::vector<Tree> trees;
         *      auto tf1 = [&trees] () -> Tree & {
         *          trees.emplace_back();
         *          return trees.back();
         *      };
         *
         *      // Container (vector) of pointers to Tree objects
         *      std::vector<std::shared_ptr<Tree>> tree_ptrs;
         *      auto tf2 = [&tree_ptrs] () -> Tree & {
         *          tree_ptrs.emplace_back(std::shared_ptr<Tree>(new Tree()));
         *          return *tree_ptrs.back();
         *      };
         *
         *      // Single tree object (if only one tree is needed)
         *      Tree tree;
         *      auto tf3 = [&tree] () -> Tree & { return tree; }
         *
         * @param tree_limit
         *   The maximum number of trees to be read from the source. If 0, all
         *   trees available in the source will be read.
         *
         * @return
         *   The number of trees read.
         */
        virtual unsigned long read(
                std::istream & src,
                const std::function<TreeT & ()> & get_new_tree_reference,
                unsigned long tree_limit=0) {
            return this->parse_stream(src, get_new_tree_reference, tree_limit);
        }

        // overload for binding `src` to temporary
        virtual unsigned long read(
                std::istream && src,
                const std::function<TreeT & ()> & get_new_tree_reference,
                unsigned long tree_limit=0) {
            return this->read(src, get_new_tree_reference, tree_limit);
        }

        std::vector<TreeT> get_tree_vector(
                std::istream & src,
                unsigned long tree_limit=0) {
                // std::function<TreeT&()> new_tree_fn = []()->TreeT&{return TreeT();}) {
            std::vector<TreeT> trees;
            auto tf = [&trees]()->TreeT & { trees.emplace_back(); return trees.back(); };
            this->parse_stream(src, tf, tree_limit);
            return trees;
        }

        std::vector<std::unique_ptr<TreeT>> get_tree_ptr_vector(
                std::istream& src,
                unsigned long tree_limit=0) {
                // std::function<TreeT&()> new_tree_fn = []()->TreeT&{return TreeT();}) {
            std::vector<std::unique_ptr<TreeT>> trees;
            // auto tf = [&trees, &new_tree_fn]()->TreeT & { trees.emplace_back(new_tree_fn()); return trees.back(); };
            auto tf = [&trees]()->TreeT & { std::unique_ptr<TreeT> t(new TreeT()); trees.push_back(t); return *trees.back(); };
            this->parse_stream(src, tf, tree_limit);
            return trees;
        }

    protected:

        // To be implementad by derived classes.
        virtual unsigned long parse_stream(
                std::istream& src,
                const std::function<TreeT & ()> & get_new_tree_reference,
                unsigned long tree_limit=0) = 0;

}; // BaseTreeReader

} // namespace platypus

#endif
