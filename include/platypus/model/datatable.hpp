/**
 * @package     platypus-phyloinformary
 * @brief       A two-dimensional container with columns of varying
 *              (signed integer, unsigned integer, real, or string) types.
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

#ifndef PLATYPUS_UTILITY_DATATABLE_HPP
#define PLATYPUS_UTILITY_DATATABLE_HPP

#include <memory>
#include <map>
#include <set>
#include <sstream>
#include <typeinfo>
#include <stdexcept>
#include <functional>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <initializer_list>
#include "../base/exception.hpp"

namespace platypus {

//////////////////////////////////////////////////////////////////////////////
// Errors

class DataTableException : public PlatypusException {
    public:
        DataTableException(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : PlatypusException(filename, line_num, message) { }
};


class DataTableUndefinedColumnError : public DataTableException {
    public:
        DataTableUndefinedColumnError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableException(filename, line_num, message) { }
};

class DataTableInvalidCellError : public DataTableException {
    public:
        DataTableInvalidCellError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableException(filename, line_num, message) { }
};

class DataTableUndefinedColumnValueType : public DataTableException {
    public:
        DataTableUndefinedColumnValueType(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableException(filename, line_num, message) { }
};

class DataTableStructureError : public DataTableException {
    public:
        DataTableStructureError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableStructureError(filename, line_num, message) { }
};

class DataTableInvalidRowError : public DataTableException {
    public:
        DataTableInvalidRowError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableException(filename, line_num, message) { }
};

namespace datatable { // implementation details supporting DataTable

//////////////////////////////////////////////////////////////////////////////
// OutputStreamManipulators

// from:: http://stackoverflow.com/questions/14702629/can-you-pass-a-manipulator-to-a-function

// an abstract class to provide a common interface to all manipulators
class abstract_manip {
    public:
        virtual ~abstract_manip() { }
        virtual void apply(std::ostream& out) const = 0;
};

// a wrapper template to let arbitrary manipulators follow that interface
template<typename M> class concrete_manip : public abstract_manip {
    public:
        concrete_manip(const M& manip) : _manip(manip) { }
        void apply(std::ostream& out) const { out << _manip; }
    private:
        M _manip;
};

// a class to hide the memory management required for polymorphism
class smanip {
    public:
        template<typename M> smanip(const M& manip)
            : _manip(new concrete_manip<M>(manip)) { }
        template<typename R, typename A> smanip(R (&manip)(A))
            : _manip(new concrete_manip<R (*)(A)>(&manip)) { }
        void apply(std::ostream& out) const { _manip->apply(out); }
    private:
        std::shared_ptr<abstract_manip> _manip;
};

inline std::ostream& operator<<(std::ostream& out, const smanip& manip) {
    manip.apply(out);
    return out;
}

typedef smanip OutputStreamManipulator;
typedef std::vector<OutputStreamManipulator>    OutputStreamManipulators;

//////////////////////////////////////////////////////////////////////////////
// DataTableColumn

class DataTableColumn {
    public:

        enum class ValueType {
                SignedInteger,
                UnsignedInteger,
                FloatingPoint,
                String };
        static const std::string & get_value_type_name_as_string(DataTableColumn::ValueType vt) {
            static const std::vector<std::string> value_type_names{"SignedInteger", "UnsignedInteger", "FloatingPoint", "String"};
            return value_type_names[static_cast<typename std::underlying_type<DataTableColumn::ValueType>::type>(vt)];
        }

        typedef long             signed_integer_implementation_type;
        typedef unsigned long    unsigned_integer_implementation_type;
        typedef long double      floating_point_implementation_type;
        typedef std::string      string_implementation_type;
        template <class T> static ValueType identify_type() {
            if (std::is_floating_point<T>::value) {
                return ValueType::FloatingPoint;
            } else if (std::is_arithmetic<T>::value) {
                if (std::is_signed<T>::value) {
                    return ValueType::SignedInteger;
                } else {
                    return ValueType::UnsignedInteger;
                }
            } else {
                return ValueType::String;
            }
        }
    public:
        DataTableColumn(ValueType value_type,
                const std::string & label,
                const OutputStreamManipulators & format_manipulators={})
                : value_type_(value_type)
                , label_(label)
                , format_manipulators_(format_manipulators) {
        }
        const std::string & get_label() const {
            return this->label_;
        }
        ValueType get_value_type() const {
            return this->value_type_;
        }
        template <class T>
        void add_formatting(const T & format_manipulator) {
            this->format_manipulators_.push_back(format_manipulator);
        }
        void add_formatting(const OutputStreamManipulators & format_manipulators) {
            this->format_manipulators_.insert(this->format_manipulators_.end(), format_manipulators.cbegin(), format_manipulators.cend());
        }
        void clear_formatting() {
            this->format_manipulators_.clear();
        }
        void set_formatting(const OutputStreamManipulators & format_manipulators) {
            this->format_manipulators_ = format_manipulators;
            // this->format_manipulators_.clear();
            // this->format_manipulators_.insert(this->format_manipulators_.end(), format_manipulators.cbegin(), format_manipulators.cend());
        }
        template <class T> void print_value(std::ostream & out, const T & val) const {
            for (auto m : this->format_manipulators_) {
                out << m;
            }
            out << val;
            out.copyfmt(std::ios(NULL)); // restore state
        }
    protected:
        ValueType                   value_type_;
        std::string                 label_;
        OutputStreamManipulators    format_manipulators_;
}; // DataTableColumn

//////////////////////////////////////////////////////////////////////////////
// BaseCell

class BaseCell {
    public:
        BaseCell(const DataTableColumn & column)
            : column_(column) { }
        virtual ~BaseCell() { }
        const DataTableColumn & column() const {
            return this->column_;
        }
    protected:
        const DataTableColumn &    column_;
}; // BaseCell

//////////////////////////////////////////////////////////////////////////////
// TypedCell

template <class T>
class TypedCell : public BaseCell {
    public:
        TypedCell(const DataTableColumn & column)
            : BaseCell(column) { }
        TypedCell(const DataTableColumn & column, const T & val)
            : BaseCell(column)
            , value_(val) { }
        virtual ~TypedCell() {}
        void print(std::ostream & out) const {
            this->column_.print_value(out, this->value_);
        }
    public:
        T       value_;
}; // TypedCell

//////////////////////////////////////////////////////////////////////////////
// NumericCell

template <class T>
class NumericCell : public TypedCell<T> {
    public:
        NumericCell(const DataTableColumn & column)
            : TypedCell<T>(column) { }
        NumericCell(const DataTableColumn & column, const T & val)
            : TypedCell<T>(column, val) { }
        virtual ~NumericCell() {}
        virtual void from_string(const std::string & val) {
            std::istringstream i(val);
            i >> this->value_;
        }
        virtual std::string to_string() const {
            std::ostringstream o;
            this->column_.print_value(o, this->value_);
            std::istringstream i(o.str());
            return o.str();
        }
        template <class U> U get() const {
            return static_cast<U>(this->value_);
        }
        void print(std::ostream & out) const {
            this->column_.print_value(out, this->value_);
        }
        virtual void set(short int val) { this->value_ = static_cast<T>(val); }
        virtual void set(unsigned short int val) { this->value_ = static_cast<T>(val); }
        virtual void set(int val) { this->value_ = static_cast<T>(val); }
        virtual void set(unsigned int val) { this->value_ = static_cast<T>(val); }
        virtual void set(long int val) { this->value_ = static_cast<T>(val); }
        virtual void set(unsigned long int val) { this->value_ = static_cast<T>(val); }
        virtual void set(long long int val) { this->value_ = static_cast<T>(val); }
        virtual void set(unsigned long long int val) { this->value_ = static_cast<T>(val); }
        virtual void set(float val) { this->value_ = static_cast<T>(val); }
        virtual void set(double val) { this->value_ = static_cast<T>(val); }
        virtual void set(long double val) { this->value_ = static_cast<T>(val); }
        virtual void set(const std::string & val) { this->from_string(val); }
}; // NumericCell

//////////////////////////////////////////////////////////////////////////////
// SignedIntegerCell

class SignedIntegerCell : public NumericCell<DataTableColumn::signed_integer_implementation_type> {
    public:
        typedef DataTableColumn::signed_integer_implementation_type value_implementation_type;
        SignedIntegerCell(const DataTableColumn & column)
            : NumericCell<value_implementation_type>(column, 0) {
        }
        template <class U> U get() const {
            return static_cast<U>(this->value_);
        }
}; // specialization for long
template <> inline std::string SignedIntegerCell::get() const { return this->to_string(); }

//////////////////////////////////////////////////////////////////////////////
// UnsignedIntegerCell

class UnsignedIntegerCell : public NumericCell<DataTableColumn::unsigned_integer_implementation_type> {
    public:
        typedef DataTableColumn::unsigned_integer_implementation_type value_implementation_type;
        UnsignedIntegerCell(const DataTableColumn & column)
            : NumericCell<value_implementation_type>(column, 0) {
        }
        template <class U> U get() const {
            return static_cast<U>(this->value_);
        }
}; // specialization for unsigned long
template <> inline std::string UnsignedIntegerCell::get() const { return this->to_string(); }

//////////////////////////////////////////////////////////////////////////////
// FloatingPointCell

class FloatingPointCell : public NumericCell<DataTableColumn::floating_point_implementation_type> {
    public:
        typedef DataTableColumn::floating_point_implementation_type value_implementation_type;
        FloatingPointCell(const DataTableColumn & column)
            : NumericCell<value_implementation_type>(column, 0.0) {
        }
        template <class U> U get() const {
            return static_cast<U>(this->value_);
        }
}; // specialization for double
template <> inline std::string FloatingPointCell::get() const { return this->to_string(); }

//////////////////////////////////////////////////////////////////////////////
// StringCell

class StringCell : public TypedCell<DataTableColumn::string_implementation_type> {
    public:
        typedef DataTableColumn::string_implementation_type value_implementation_type;

        StringCell(const DataTableColumn & column)
            : TypedCell<value_implementation_type>(column) { }

        template <class U> void set(const U & val) {
            std::ostringstream o;
            this->column_.print_value(o, val);
            this->value_ = o.str();
        }
        void set(char * val) {
            this->value_ = val;
        }
        void set(const char * val) {
            this->value_ = val;
        }
        template <class U> U get() const {
            U u;
            std::istringstream i(this->value_);
            i >> u;
            return u;
        }
        void print(std::ostream & out) const {
            this->column_.print_value(out, this->value_);
        }
}; // specialization for std::string
template <> inline std::string StringCell::get() const { return this->value_; }
template <> inline void StringCell::set(const std::string & val) { this->value_ = val; }

//////////////////////////////////////////////////////////////////////////////
// DataTableRow

class DataTableRow {

    public:
        DataTableRow(
                std::vector<DataTableColumn *> & columns,
                std::map<std::string, unsigned long> & column_label_index_map)
                : columns_(columns)
                , column_label_index_map_(column_label_index_map)
                , current_entry_cell_idx_(0) {
            this->create_cells();
        }

        ~DataTableRow() {
            for (auto & c : this->cells_) {
                delete c;
            }
        }

        template <class T>
        const T get(unsigned long column_idx) const {
            if (column_idx >= this->cells_.size()) {
                throw DataTableInvalidCellError(__FILE__, __LINE__, "column index is out of bounds");
            }
            auto * cell = this->cells_[column_idx];
            return DataTableRow::get_cell_value<T>(cell);
        }

        template <class T> T get(const std::string & col_name) const {
            auto citer = this->column_label_index_map_.find(col_name);
            if (citer == this->column_label_index_map_.end()) {
                throw DataTableUndefinedColumnError(__FILE__, __LINE__, col_name);
            }
            return DataTableRow::get_cell_value<T>(this->cells_[citer->second]);
        }

        template <class T>
        DataTableRow & operator<<(const T & val) {
            if (this->current_entry_cell_idx_ >= this->cells_.size()) {
                throw DataTableInvalidCellError(__FILE__, __LINE__, "attempting to add data beyond end of row");
            }
            auto * cell = this->cells_[this->current_entry_cell_idx_];
            this->set_cell_value(cell, val);
            ++this->current_entry_cell_idx_;
            return * this;
        }

        template <class T>
        void set(unsigned long column_idx, const T & val) {
            if (column_idx >= this->cells_.size()) {
                throw DataTableInvalidCellError(__FILE__, __LINE__, "column index is out of bounds");
            }
            auto * cell = this->cells_[column_idx];
            this->set_cell_value(cell, val);
        }

        template <class T>
        void set(const std::string & col_name, const T & val) {
            auto citer = this->column_label_index_map_.find(col_name);
            if (citer == this->column_label_index_map_.end()) {
                throw DataTableUndefinedColumnError(__FILE__, __LINE__, col_name);
            }
            return DataTableRow::get_cell_value<T>(this->cells_[citer->second]);
            this->set_cell_value(this->cells_[citer->second], val);
        }

        //////////////////////////////////////////////////////////////////////////////
        // Iteration

        template <class ValueT, class IterT>
        class iterator {
            public:
				typedef iterator                     self_type;
				typedef ValueT                       value_type;
				typedef value_type *                 pointer;
				typedef const value_type *           const_pointer;
				typedef value_type &                 reference;
				typedef const value_type &           const_reference;
				typedef unsigned long                size_type;
				typedef int                          difference_type;
				typedef std::forward_iterator_tag    iterator_category;
			public:
                iterator(IterT cell_iter, IterT cell_end)
                        : cell_iter_(cell_iter)
                        , cell_end_(cell_end) {
                    if (this->cell_iter_ != this->cell_end_) {
                        this->current_value_ = DataTableRow::get_cell_value<ValueT>(*this->cell_iter_);
                    }
                }
                virtual ~iterator() {
                }
                inline const_reference operator*() {
                    return this->current_value_;
                }
                inline const_pointer operator->() {
                    return &(this->current_value_);
                }
                inline bool operator==(const self_type& rhs) const {
                    return this->cell_iter_ == rhs.cell_iter_;
                }
                inline bool operator!=(const self_type& rhs) const {
                    return !(*this == rhs);
                }
                inline const self_type & operator++() {
                // inline self_type operator++() {
                    if (this->cell_iter_ != this->cell_end_) {
                        ++this->cell_iter_;
                        if (this->cell_iter_ != this->cell_end_) {
                            this->current_value_ = DataTableRow::get_cell_value<ValueT>(*this->cell_iter_);
                        }
                    }
                    return *this;
                }
                inline self_type operator++(int) {
                    self_type i = *this;
                    ++(*this);
                    return i;
                }
                template <class U>
                inline void set(const U & val) {
                    if (this->cell_iter_ != this->cell_end_) {
                        DataTableRow::set_cell_value(*this->cell_iter_, val);
                        this->current_value_ = DataTableRow::get_cell_value<ValueT>(*this->cell_iter_);
                    } else {
                        throw DataTableInvalidCellError(__FILE__, __LINE__, "cell index is out of bounds");
                    }
                }
            protected:
                IterT   cell_iter_;
                IterT   cell_end_;
                ValueT  current_value_;
        }; // iterator

        template <class ValueT=std::string>
        iterator<ValueT, std::vector<BaseCell *>::iterator> begin() {
            return iterator<ValueT, std::vector<BaseCell *>::iterator>(this->cells_.begin(), this->cells_.end());
        }

        template <class ValueT=std::string>
        iterator<ValueT, std::vector<BaseCell *>::iterator> end() {
            return iterator<ValueT, std::vector<BaseCell *>::iterator>(this->cells_.end(), this->cells_.end());
        }

        //////////////////////////////////////////////////////////////////////////////
        // Output

        void print(std::ostream & out, std::string separator="\t") {
            for(unsigned long cell_idx = 0; cell_idx < this->cells_.size(); ++cell_idx) {
                if (cell_idx > 0) {
                    out << separator;
                }
                // out << *(this->cells_[cell_idx]);
                DataTableRow::print_cell(out, this->cells_[cell_idx]);
            }
            out << std::endl;
        }

    public:

        template <class T>
        static const T get_cell_value(BaseCell * cell) {
            auto & column_definition = cell->column();
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == DataTableColumn::ValueType::SignedInteger) {
                SignedIntegerCell * t = dynamic_cast<SignedIntegerCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == DataTableColumn::ValueType::UnsignedInteger) {
                UnsignedIntegerCell * t = dynamic_cast<UnsignedIntegerCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == DataTableColumn::ValueType::FloatingPoint) {
                FloatingPointCell * t = dynamic_cast<FloatingPointCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == DataTableColumn::ValueType::String) {
                StringCell * t = dynamic_cast<StringCell *>(cell);
                return t->get<T>();
            } else {
                throw DataTableUndefinedColumnValueType(__FILE__, __LINE__, DataTableColumn::get_value_type_name_as_string(cell_value_type));
            }
        }

        template <class T>
        static void set_cell_value(BaseCell * cell, const T & val) {
            auto & column_definition = cell->column();
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == DataTableColumn::ValueType::SignedInteger) {
                SignedIntegerCell * t = dynamic_cast<SignedIntegerCell *>(cell);
                t->set(val);
            } else if (cell_value_type == DataTableColumn::ValueType::UnsignedInteger) {
                UnsignedIntegerCell * t = dynamic_cast<UnsignedIntegerCell *>(cell);
                t->set(val);
            } else if (cell_value_type == DataTableColumn::ValueType::FloatingPoint) {
                FloatingPointCell * t = dynamic_cast<FloatingPointCell *>(cell);
                t->set(val);
            } else if (cell_value_type == DataTableColumn::ValueType::String) {
                StringCell * t = dynamic_cast<StringCell *>(cell);
                t->set(val);
            } else {
                throw DataTableUndefinedColumnValueType(__FILE__, __LINE__, DataTableColumn::get_value_type_name_as_string(cell_value_type));
            }
        }

        static void print_cell(std::ostream & out, BaseCell * cell) {
            auto & column_definition = cell->column();
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == DataTableColumn::ValueType::SignedInteger) {
                SignedIntegerCell * t = dynamic_cast<SignedIntegerCell *>(cell);
                t->print(out);
            } else if (cell_value_type == DataTableColumn::ValueType::UnsignedInteger) {
                UnsignedIntegerCell * t = dynamic_cast<UnsignedIntegerCell *>(cell);
                t->print(out);
            } else if (cell_value_type == DataTableColumn::ValueType::FloatingPoint) {
                FloatingPointCell * t = dynamic_cast<FloatingPointCell *>(cell);
                t->print(out);
            } else if (cell_value_type == DataTableColumn::ValueType::String) {
                StringCell * t = dynamic_cast<StringCell *>(cell);
                t->print(out);
            } else {
                throw DataTableUndefinedColumnValueType(__FILE__, __LINE__, DataTableColumn::get_value_type_name_as_string(cell_value_type));
            }
        }

    protected:
        void create_cells() {
            this->current_entry_cell_idx_ = 0;
            this->cells_.reserve(this->columns_.size());
            for (auto & column : this->columns_) {
                auto cell_value_type = column->get_value_type();
                BaseCell * new_cell = nullptr;
                if (cell_value_type == DataTableColumn::ValueType::SignedInteger) {
                    new_cell = new SignedIntegerCell(*column);
                } else if (cell_value_type == DataTableColumn::ValueType::UnsignedInteger) {
                    new_cell = new UnsignedIntegerCell(*column);
                } else if (cell_value_type == DataTableColumn::ValueType::FloatingPoint) {
                    new_cell = new FloatingPointCell(*column);
                } else if (cell_value_type == DataTableColumn::ValueType::String) {
                    new_cell = new StringCell(*column);
                } else {
                    throw DataTableUndefinedColumnValueType(__FILE__, __LINE__, DataTableColumn::get_value_type_name_as_string(cell_value_type));
                }
                this->cells_.push_back(new_cell);
            }
        }

    private:
        std::vector<DataTableColumn *> &          columns_;
        std::map<std::string, unsigned long> &    column_label_index_map_;
        std::vector<BaseCell *>                   cells_;
        unsigned long                             current_entry_cell_idx_;

}; // DataTableRow

} // namespace datatable

//////////////////////////////////////////////////////////////////////////////
// Table

class DataTable {

    public:
        typedef datatable::DataTableColumn             Column;
        typedef datatable::DataTableRow                Row;
        typedef datatable::OutputStreamManipulator     OutputStreamManipulator;
        typedef datatable::OutputStreamManipulators    OutputStreamManipulators;

    public:
        DataTable() {
        }
        ~DataTable() {
            for (auto & r : this->rows_) {
                delete r;
            }
            for (auto & c : this->columns_) {
                delete c;
            }
        }
        template <class T> Column & add_column(const std::string & label,
                const OutputStreamManipulators & format_manipulators={},
                bool is_key_column=false) {
            if (is_key_column) {
                return this->add_key_column<T>(label, format_manipulators);
            } else {
                return this->add_data_column<T>(label, format_manipulators);
            }
        }
        template <class T> Column & add_key_column(const std::string & label, const OutputStreamManipulators & format_manipulators={}) {
            auto & col = this->create_column<T>(label, format_manipulators);
            this->key_columns_.push_back(&col);
            return col;
        }
        template <class T> Column & add_data_column(const std::string & label, const OutputStreamManipulators & format_manipulators={}) {
            auto & col = this->create_column<T>(label, format_manipulators);
            this->data_columns_.push_back(&col);
            return col;
        }
        Row & add_row() {
            Row * r = new Row(this->columns_, this->column_label_index_map_);
            this->rows_.push_back(r);
            return *r;
        }
        unsigned long num_columns() const {
            return this->column_label_index_map_.size();
        }
        unsigned long num_rows() const {
            return this->rows_.size();
        }
        Row & operator[](unsigned long ridx) {
            if (ridx >= this->rows_.size()) {
                throw DataTableInvalidRowError(__FILE__, __LINE__, "row index is out of bounds");
            }
            return *this->rows_[ridx];
        }
        template <class T> T get(unsigned long ridx, const std::string & col_name) {
            if (ridx >= this->rows_.size()) {
                throw DataTableInvalidRowError(__FILE__, __LINE__, "row index is out of bounds");
            }
            return this->rows_[ridx]->get<T>(col_name);
        }
        template <class T> T get(unsigned long ridx, unsigned long cidx) {
            if (ridx >= this->rows_.size()) {
                throw DataTableInvalidRowError(__FILE__, __LINE__, "row index is out of bounds");
            }
            return this->rows_[ridx]->get<T>(cidx);
        }
        Column & column(unsigned long column_idx) {
            if (column_idx >= this->columns_.size()) {
                throw DataTableInvalidCellError(__FILE__, __LINE__, "column index is out of bounds");
            }
            return *(this->columns_[column_idx]);
        }
        Column & column(const std::string & col_name) {
            auto citer = this->column_label_index_map_.find(col_name);
            if (citer == this->column_label_index_map_.end()) {
                throw DataTableUndefinedColumnError(__FILE__, __LINE__, col_name);
            }
            return *(this->columns_[citer->second]);
        }
        const Column & column(unsigned long column_idx) const {
            return this->column(column_idx);
        }

        //////////////////////////////////////////////////////////////////////////////
        // Iteration

        template <class IterT>
        class iterator {
            public:
				typedef iterator                     self_type;
				typedef Row                          value_type;
				typedef value_type *                 pointer;
				typedef const value_type *           const_pointer;
				typedef value_type &                 reference;
				typedef const value_type &           const_reference;
				typedef unsigned long                size_type;
				typedef int                          difference_type;
				typedef std::forward_iterator_tag    iterator_category;
			public:
                iterator(IterT row_iter, IterT row_end)
                        : row_iter_(row_iter)
                        , row_end_(row_end) {
                }
                virtual ~iterator() {
                }
                inline reference operator*() {
                    return **this->row_iter_;
                }
                inline pointer operator->() {
                    return &(**this->row_iter_);
                }
                inline bool operator==(const self_type& rhs) const {
                    return this->row_iter_ == rhs.row_iter_;
                }
                inline bool operator!=(const self_type& rhs) const {
                    return !(*this == rhs);
                }
                inline const self_type & operator++() {
                // inline self_type operator++() {
                    if (this->row_iter_ != this->row_end_) {
                        ++this->row_iter_;
                    }
                    return *this;
                }
                inline self_type operator++(int) {
                    self_type i = *this;
                    ++(*this);
                    return i;
                }
            protected:
                IterT   row_iter_;
                IterT   row_end_;
        }; // iterator

        iterator<std::vector<Row *>::iterator> begin() {
            return iterator<std::vector<Row *>::iterator>(this->rows_.begin(), this->rows_.end());
        }

        iterator<std::vector<Row *>::iterator> end() {
            return iterator< std::vector<Row *>::iterator>(this->rows_.end(), this->rows_.end());
        }

    private:
        template <class T> Column & create_column(
                const std::string & label,
                const OutputStreamManipulators & format_manipulators={}) {
            if (!this->rows_.empty()) {
                throw DataTableStructureError(__FILE__, __LINE__, "Cannot add new column: rows have already been added");
            }
            if (this->column_label_index_map_.find(label) != this->column_label_index_map_.end()) {
                throw DataTableStructureError(__FILE__, __LINE__, "Cannot add new column: duplicate column name");
            }
            auto new_col = new Column(Column::identify_type<T>(), label, format_manipulators);
            this->column_label_index_map_[label] = this->columns_.size();
            this->columns_.push_back(new_col);
            return *new_col;
        }
    private:
        std::vector<Column *>                   columns_;
        std::map<std::string, unsigned long>    column_label_index_map_;
        std::vector<Column *>                   key_columns_;
        std::vector<Column *>                   data_columns_;
        std::vector<Row *>                      rows_;
}; // DataTable

} // namespace platypus

#endif
