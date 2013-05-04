/**
 * @package     platypus-phyloinformary
 * @brief       Base class for classes that serialize phylogenetic data
 *              objects.
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

#ifndef PLATYPUS_BASE_WRITER_BASE_HPP
#define PLATYPUS_BASE_WRITER_BASE_HPP

#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <functional>
#include "exception.hpp"

namespace platypus {

////////////////////////////////////////////////////////////////////////////////
// WriterException

class WriterException : public PlatypusException {
    public:
        WriterException(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : PlatypusException(filename, line_num, message) { }
};

////////////////////////////////////////////////////////////////////////////////
// BaseTreeWriter

/**
 * @brief Formats/composes phylogenetic data objects and writes to streams,
 * files and strings.
 *
 * @tparam TreeT
 *   A class representing a specialization or a derived specialization of
 *   platypus::Tree.
 *
 */
template <typename TreeT, typename EdgeLengthT=double>
class BaseTreeWriter {

    public:

        // typdefs for data
        typedef TreeT                            tree_type;
        typedef typename tree_type::node_type    tree_node_type;
        typedef typename tree_type::value_type   tree_value_type;

        // typedefs for functions used in writing
        typedef std::function<bool (const tree_type &)>                 tree_is_rooted_getter_fntype;
        typedef std::function<std::string (const tree_value_type &)>    node_value_label_getter_fntype;
        typedef std::function<EdgeLengthT (const tree_value_type &)>    node_value_edge_length_getter_fntype;

    public:
        BaseTreeWriter() { }
        virtual ~BaseTreeWriter() { }

        // Setting/binding of functions
        virtual void set_tree_is_rooted_getter(const tree_is_rooted_getter_fntype & tree_is_rooted_func) {
            this->tree_is_rooted_getter_ = tree_is_rooted_func;
        }
        virtual void set_node_label_getter(const node_value_label_getter_fntype & node_value_label_func) {
            this->node_value_label_getter_ = node_value_label_func;
        }
        virtual void set_edge_length_getter(const node_value_edge_length_getter_fntype & node_value_edge_length_func) {
            this->node_value_edge_length_getter_ = node_value_edge_length_func;
        }

        // Use of functions
        // virtual bool get_tree_is_rooted(const tree_type & tree) const {
        //     if (this->tree_is_rooted_getter_) {
        //         return this->tree_is_rooted_getter_(tree);
        //     }
        // }
        // virtual std::string get_node_value_label(tree_value_type & nv) const {
        //     if (this->node_value_label_getter_) {
        //         return this->node_value_label_getter_(nv);
        //     }
        // }
        // virtual double get_node_value_edge_length(tree_value_type & nv) const {
        //     if (this->node_value_edge_length_getter_) {
        //         return this->node_value_edge_length_getter_(nv);
        //     }
        // }

        // Control of writing
        unsigned int get_edge_length_precision() const {
            return this->edge_length_precision_;
        }
        void set_edge_length_precision(unsigned int prec) {
            this->edge_length_precision_ = prec;
        }

    protected:
        tree_is_rooted_getter_fntype                tree_is_rooted_getter_;
        node_value_label_getter_fntype              node_value_label_getter_;
        node_value_edge_length_getter_fntype        node_value_edge_length_getter_;
        unsigned int                                edge_length_precision_;

}; // BaseTreeWriter

} // namespace platypus

#endif
