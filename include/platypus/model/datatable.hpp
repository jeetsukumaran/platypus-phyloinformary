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

#ifndef PLATYPUS_MODEL_DATATABLE_HPP
#define PLATYPUS_MODEL_DATATABLE_HPP

#include <numeric>
#include <cmath>
#include <algorithm>
#include <map>
#include <set>
#include <sstream>
#include <typeinfo>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <initializer_list>
#include "../utility/stream.hpp"
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
            : DataTableException(filename, line_num, "Undefined column: " + message) { }
};

class DataTableInvalidCellError : public DataTableException {
    public:
        DataTableInvalidCellError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableException(filename, line_num, "Invalid cell: " + message) { }
};

class DataTableUndefinedColumnValueType : public DataTableException {
    public:
        DataTableUndefinedColumnValueType(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableException(filename, line_num, "Undefined column value type: " + message) { }
};

class DataTableStructureError : public DataTableException {
    public:
        DataTableStructureError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableException(filename, line_num, message) { }
};

class DataTableInvalidRowError : public DataTableException {
    public:
        DataTableInvalidRowError(
                    const std::string & filename,
                    unsigned long line_num,
                    const std::string & message)
            : DataTableException(filename, line_num, message) { }
};

//////////////////////////////////////////////////////////////////////////////
// DataTableColumn

class DataTable;

class DataTableColumn {
    public:

        enum class ValueType {
                SignedInteger,
                UnsignedInteger,
                FloatingPoint,
                String };
        static const std::string & get_value_type_name_as_string(DataTableColumn::ValueType vt) {
            static const std::vector<std::string> value_type_names{"SignedInteger", "UnsignedInteger", "FloatingPoint", "String"};
            return value_type_names.at(static_cast<typename std::underlying_type<DataTableColumn::ValueType>::type>(vt));
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
        DataTableColumn(
                DataTable & table,
                ValueType value_type,
                const std::string & label,
                bool is_key_column=false,
                const platypus::stream::OutputStreamFormatters & formatters={})
                : table_(table)
                , value_type_(value_type)
                , label_(label)
                , is_key_column_(is_key_column)
                , formatters_(formatters)
                , is_hidden_(false) {
        }
        const std::string & get_label() const {
            return this->label_;
        }
        ValueType get_value_type() const {
            return this->value_type_;
        }
        bool is_key_column() const {
            return this->is_key_column_;
        }
        void set_key_column(bool v) {
            this->is_key_column_ = v;
        }
        template <class T>
        void add_formatting(const T & formatter) {
            this->formatters_.push_back(formatter);
        }
        void add_formatting(const platypus::stream::OutputStreamFormatters & formatters) {
            this->formatters_.insert(this->formatters_.end(), formatters.cbegin(), formatters.cend());
        }
        void clear_formatting() {
            this->formatters_.clear();
        }
        void set_formatting(const platypus::stream::OutputStreamFormatters & formatters) {
            this->formatters_ = formatters;
            // this->formatters_.clear();
            // this->formatters_.insert(this->formatters_.end(), formatters.cbegin(), formatters.cend());
        }
        const platypus::stream::OutputStreamFormatters & get_formatting() const {
            return this->formatters_;
        }
        template <class T> void write_formatted_value(std::ostream & out, const T & val) const {
            for (auto m : this->formatters_) {
                out << m;
            }
            out << val;
            out.copyfmt(std::ios(NULL)); // restore state
        }
        bool is_hidden() const {
            return this->is_hidden_;
        }
        void set_hidden(bool hidden) {
            this->is_hidden_ = hidden;
        }
        template <class T> T max() const;
    protected:
        DataTable &                                 table_;
        ValueType                                   value_type_;
        std::string                                 label_;
        bool                                        is_key_column_;
        platypus::stream::OutputStreamFormatters    formatters_;
        bool                                        is_hidden_;
}; // DataTableColumn

//////////////////////////////////////////////////////////////////////////////
// DataTableBaseCell

class DataTableBaseCell {
    public:
        DataTableBaseCell(const DataTableColumn & column)
            : column_(column) { }
        virtual ~DataTableBaseCell() { }
        const DataTableColumn & column() const {
            return this->column_;
        }
    protected:
        const DataTableColumn &    column_;
}; // DataTableBaseCell

//////////////////////////////////////////////////////////////////////////////
// DataTableValueTypedCell

template <class T>
class DataTableValueTypedCell : public DataTableBaseCell {
    public:
        DataTableValueTypedCell(const DataTableColumn & column)
            : DataTableBaseCell(column) { }
        DataTableValueTypedCell(const DataTableColumn & column, const T & val)
            : DataTableBaseCell(column)
            , value_(val) { }
        virtual ~DataTableValueTypedCell() {}
        void write_formatted(std::ostream & out) const {
            this->column_.write_formatted_value(out, this->value_);
        }
        std::string get_formatted_value() const {
            std::ostringstream o;
            this->write_formatted_value(o);
            return o.str();
        }
    public:
        T       value_;
}; // DataTableValueTypedCell

//////////////////////////////////////////////////////////////////////////////
// DataTableNumericCell

template <class T>
class DataTableNumericCell : public DataTableValueTypedCell<T> {
    public:
        typedef T value_implementation_type;
        DataTableNumericCell(const DataTableColumn & column)
            : DataTableValueTypedCell<T>(column) { }
        DataTableNumericCell(const DataTableColumn & column, const T & val)
            : DataTableValueTypedCell<T>(column, val) { }
        virtual ~DataTableNumericCell() {}
        virtual void from_string(const std::string & val) {
            std::istringstream i(val);
            i >> this->value_;
        }
        virtual std::string to_string() const {
            std::ostringstream o;
            this->column_.write_formatted_value(o, this->value_);
            std::istringstream i(o.str());
            return o.str();
        }
        template <class U> U get() const {
            return static_cast<U>(this->value_);
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

}; // DataTableNumericCell

//////////////////////////////////////////////////////////////////////////////
// DataTableSignedIntegerCell

class DataTableSignedIntegerCell : public DataTableNumericCell<DataTableColumn::signed_integer_implementation_type> {
    public:
        typedef DataTableColumn::signed_integer_implementation_type value_implementation_type;
        DataTableSignedIntegerCell(const DataTableColumn & column)
            : DataTableNumericCell<value_implementation_type>(column, 0) {
        }
        template <class U> U get() const {
            return static_cast<U>(this->value_);
        }
}; // specialization for long
template <> inline std::string DataTableSignedIntegerCell::get() const { return this->to_string(); }

//////////////////////////////////////////////////////////////////////////////
// DataTableUnsignedIntegerCell

class DataTableUnsignedIntegerCell : public DataTableNumericCell<DataTableColumn::unsigned_integer_implementation_type> {
    public:
        typedef DataTableColumn::unsigned_integer_implementation_type value_implementation_type;
        DataTableUnsignedIntegerCell(const DataTableColumn & column)
            : DataTableNumericCell<value_implementation_type>(column, 0) {
        }
        template <class U> U get() const {
            return static_cast<U>(this->value_);
        }
}; // specialization for unsigned long
template <> inline std::string DataTableUnsignedIntegerCell::get() const { return this->to_string(); }

//////////////////////////////////////////////////////////////////////////////
// DataTableFloatingPointCell

class DataTableFloatingPointCell : public DataTableNumericCell<DataTableColumn::floating_point_implementation_type> {
    public:
        typedef DataTableColumn::floating_point_implementation_type value_implementation_type;
        DataTableFloatingPointCell(const DataTableColumn & column)
            : DataTableNumericCell<value_implementation_type>(column, 0.0) {
        }
        template <class U> U get() const {
            return static_cast<U>(this->value_);
        }
}; // specialization for double
template <> inline std::string DataTableFloatingPointCell::get() const { return this->to_string(); }

//////////////////////////////////////////////////////////////////////////////
// DataTableStringCell

class DataTableStringCell : public DataTableValueTypedCell<DataTableColumn::string_implementation_type> {
    public:
        typedef DataTableColumn::string_implementation_type value_implementation_type;

        DataTableStringCell(const DataTableColumn & column)
            : DataTableValueTypedCell<value_implementation_type>(column) { }

        template <class U> void set(const U & val) {
            std::ostringstream o;
            this->column_.write_formatted_value(o, val);
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
}; // specialization for std::string
template <> inline std::string DataTableStringCell::get() const { return this->value_; }
template <> inline void DataTableStringCell::set(const std::string & val) { this->value_ = val; }

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
                throw DataTableInvalidCellError(__FILE__, __LINE__, "column index is out of bounds: " + std::to_string(column_idx));
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
                throw DataTableInvalidCellError(__FILE__, __LINE__, "column index is out of bounds: " + std::to_string(column_idx));
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
            this->set_cell_value(this->cells_[citer->second], val);
        }

        const DataTableBaseCell & cell(unsigned long column_idx) const {
            if (column_idx >= this->cells_.size()) {
                throw DataTableInvalidCellError(__FILE__, __LINE__, "column index is out of bounds: " + std::to_string(column_idx));
            }
            return *this->cells_[column_idx];
        }

        const DataTableBaseCell & cell(const std::string & col_name) const {
            auto citer = this->column_label_index_map_.find(col_name);
            if (citer == this->column_label_index_map_.end()) {
                throw DataTableUndefinedColumnError(__FILE__, __LINE__, col_name);
            }
            return *this->cells_[citer->second];
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
        iterator<ValueT, std::vector<DataTableBaseCell *>::iterator> begin() {
            return iterator<ValueT, std::vector<DataTableBaseCell *>::iterator>(this->cells_.begin(), this->cells_.end());
        }

        template <class ValueT=std::string>
        iterator<ValueT, std::vector<DataTableBaseCell *>::iterator> end() {
            return iterator<ValueT, std::vector<DataTableBaseCell *>::iterator>(this->cells_.end(), this->cells_.end());
        }

        //////////////////////////////////////////////////////////////////////////////
        // Output

        void write_formatted(std::ostream & out, const std::string & column_separator="\t") {
            unsigned long print_idx = 0;
            for(unsigned long cell_idx = 0; cell_idx < this->cells_.size(); ++cell_idx) {
                if (!this->columns_[cell_idx]->is_hidden()) {
                    if (print_idx > 0) {
                        out << column_separator;
                    }
                    DataTableRow::write_formatted_cell(out, this->cells_[cell_idx]);
                    print_idx += 1;
                }
            }
            out << std::endl;
        }

        void write_stacked(std::ostream & out,
                std::vector<DataTableColumn *> & key_columns,
                std::vector<DataTableColumn *> & data_columns,
                const std::string & column_separator="\t") {
            unsigned long printed_idx = 0;
            for (auto & data_col : data_columns) { // each data column is itse own row
                printed_idx = 0;
                if (!data_col->is_hidden()) {
                    for (auto & key_col : key_columns) {
                        if (!key_col->is_hidden()) {
                            if (printed_idx > 0) {
                                out << column_separator;
                            }
                            DataTableRow::write_formatted_cell(out, &(this->cell(key_col->get_label())));
                            printed_idx += 1;
                        }
                    } // key colums
                    if (printed_idx > 0) {
                        out << column_separator;
                    }
                    out << data_col->get_label();
                    out << column_separator;
                    DataTableRow::write_formatted_cell(out, &(this->cell(data_col->get_label())));
                    printed_idx += 2;
                    out << "\n";
                }
            } // data columns
        }

    public:

        template <class T>
        static const T get_cell_value(DataTableBaseCell * cell) {
            auto & column_definition = cell->column();
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == DataTableColumn::ValueType::SignedInteger) {
                DataTableSignedIntegerCell * t = dynamic_cast<DataTableSignedIntegerCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == DataTableColumn::ValueType::UnsignedInteger) {
                DataTableUnsignedIntegerCell * t = dynamic_cast<DataTableUnsignedIntegerCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == DataTableColumn::ValueType::FloatingPoint) {
                DataTableFloatingPointCell * t = dynamic_cast<DataTableFloatingPointCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == DataTableColumn::ValueType::String) {
                DataTableStringCell * t = dynamic_cast<DataTableStringCell *>(cell);
                return t->get<T>();
            } else {
                throw DataTableUndefinedColumnValueType(__FILE__, __LINE__, DataTableColumn::get_value_type_name_as_string(cell_value_type));
            }
        }

        template <class T>
        static void set_cell_value(DataTableBaseCell * cell, const T & val) {
            auto & column_definition = cell->column();
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == DataTableColumn::ValueType::SignedInteger) {
                DataTableSignedIntegerCell * t = dynamic_cast<DataTableSignedIntegerCell *>(cell);
                t->set(val);
            } else if (cell_value_type == DataTableColumn::ValueType::UnsignedInteger) {
                DataTableUnsignedIntegerCell * t = dynamic_cast<DataTableUnsignedIntegerCell *>(cell);
                t->set(val);
            } else if (cell_value_type == DataTableColumn::ValueType::FloatingPoint) {
                DataTableFloatingPointCell * t = dynamic_cast<DataTableFloatingPointCell *>(cell);
                t->set(val);
            } else if (cell_value_type == DataTableColumn::ValueType::String) {
                DataTableStringCell * t = dynamic_cast<DataTableStringCell *>(cell);
                t->set(val);
            } else {
                throw DataTableUndefinedColumnValueType(__FILE__, __LINE__, DataTableColumn::get_value_type_name_as_string(cell_value_type));
            }
        }

        static void write_formatted_cell(std::ostream & out, const DataTableBaseCell * cell) {
            auto & column_definition = cell->column();
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == DataTableColumn::ValueType::SignedInteger) {
                const DataTableSignedIntegerCell * t = dynamic_cast<const DataTableSignedIntegerCell *>(cell);
                t->write_formatted(out);
            } else if (cell_value_type == DataTableColumn::ValueType::UnsignedInteger) {
                const DataTableUnsignedIntegerCell * t = dynamic_cast<const DataTableUnsignedIntegerCell *>(cell);
                t->write_formatted(out);
            } else if (cell_value_type == DataTableColumn::ValueType::FloatingPoint) {
                const DataTableFloatingPointCell * t = dynamic_cast<const DataTableFloatingPointCell *>(cell);
                t->write_formatted(out);
            } else if (cell_value_type == DataTableColumn::ValueType::String) {
                const DataTableStringCell * t = dynamic_cast<const DataTableStringCell *>(cell);
                t->write_formatted(out);
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
                DataTableBaseCell * new_cell = nullptr;
                if (cell_value_type == DataTableColumn::ValueType::SignedInteger) {
                    new_cell = new DataTableSignedIntegerCell(*column);
                } else if (cell_value_type == DataTableColumn::ValueType::UnsignedInteger) {
                    new_cell = new DataTableUnsignedIntegerCell(*column);
                } else if (cell_value_type == DataTableColumn::ValueType::FloatingPoint) {
                    new_cell = new DataTableFloatingPointCell(*column);
                } else if (cell_value_type == DataTableColumn::ValueType::String) {
                    new_cell = new DataTableStringCell(*column);
                } else {
                    throw DataTableUndefinedColumnValueType(__FILE__, __LINE__, DataTableColumn::get_value_type_name_as_string(cell_value_type));
                }
                this->cells_.push_back(new_cell);
            }
        }

    private:
        std::vector<DataTableColumn *> &          columns_;
        std::map<std::string, unsigned long> &    column_label_index_map_;
        std::vector<DataTableBaseCell *>          cells_;
        unsigned long                             current_entry_cell_idx_;

}; // DataTableRow

//////////////////////////////////////////////////////////////////////////////
// Table

class DataTable {

    public:
        typedef DataTableColumn                  Column;
        typedef DataTableRow                     Row;
        typedef platypus::stream::OutputStreamFormatter     OutputStreamFormatter;
        typedef platypus::stream::OutputStreamFormatters    OutputStreamFormatters;

        template <class T=DataTableColumn::floating_point_implementation_type>
        struct Summary {
            typedef T value_type;
            Summary()
                : size(0.0)
                , sum(0.0)
                , minimum(0.0)
                , maximum(0.0)
                , mean(0.0)
                , sample_variance(0.0)
                , population_variance(0.0) { }
            Summary(Summary && other) {
                *this = other;
            }
            Summary(const Summary & other) {
                *this = other;
            }
            Summary & operator=(const Summary & other) {
                this->size = other.size;
                this->sum = other.sum;
                this->minimum = other.minimum;
                this->maximum = other.maximum;
                this->mean = other.mean;
                this->sum_of_squares = other.sum_of_squares;
                this->sample_variance = other.sample_variance;
                this->population_variance = other.population_variance;
                return *this;
            }
            value_type  size;
            value_type  sum;
            value_type  minimum;
            value_type  maximum;
            value_type  mean;
            value_type  sum_of_squares;
            value_type  sample_variance;
            value_type  population_variance;
        };

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
                const platypus::stream::OutputStreamFormatters & formatters={},
                bool is_key_column=false) {
            if (is_key_column) {
                return this->add_key_column<T>(label, formatters);
            } else {
                return this->add_data_column<T>(label, formatters);
            }
        }
        template <class T> Column & add_key_column(const std::string & label, const platypus::stream::OutputStreamFormatters & formatters={}) {
            auto & col = this->create_column<T>(label, true, formatters);
            return col;
        }
        template <class T> Column & add_data_column(const std::string & label, const platypus::stream::OutputStreamFormatters & formatters={}) {
            auto & col = this->create_column<T>(label, false, formatters);
            return col;
        }
        Row & add_row() {
            Row * r = new Row(this->columns_,
                    this->column_label_index_map_);
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
        const Row & operator[](unsigned long ridx) const {
            return this->operator[](ridx);
        }
        Row & row(unsigned long ridx) {
            return this->operator[](ridx);
        }
        const Row & row(unsigned long ridx) const {
            return this->operator[](ridx);
        }
        Column & column(unsigned long column_idx) {
            if (column_idx >= this->columns_.size()) {
                throw DataTableInvalidCellError(__FILE__, __LINE__, "column index is out of bounds: " + std::to_string(column_idx));
            }
            return *(this->columns_[column_idx]);
        }
        const Column & column(unsigned long column_idx) const {
                                return this->column(column_idx);
        }
        Column & column(const std::string & col_name) {
            auto citer = this->column_label_index_map_.find(col_name);
            if (citer == this->column_label_index_map_.end()) {
                throw DataTableUndefinedColumnError(__FILE__, __LINE__, col_name);
            }
            return *(this->columns_[citer->second]);
        }
        const Column & column(const std::string & col_name) const {
            return this->column(col_name);
        }
        const std::vector<const Column *> column_ptrs() const {
            std::vector<const Column *> columns;
            columns.reserve(this->columns_.size());
            for (auto & col : this->columns_) {
                columns.push_back(&(*col));
            }
            return columns;
        }
        const std::vector<std::string> column_names() const {
            std::vector<std::string> column_names;
            column_names.reserve(this->columns_.size());
            for (auto & col : this->columns_) {
                column_names.push_back(col->get_label());
            }
            return column_names;
        }
        const std::vector<std::string> key_column_names() const {
            std::vector<std::string> column_names;
            column_names.reserve(this->columns_.size());
            for (auto & col : this->columns_) {
                if (col->is_key_column()) {
                    column_names.push_back(col->get_label());
                }
            }
            return column_names;
        }
        const std::vector<std::string> data_column_names() const {
            std::vector<std::string> column_names;
            column_names.reserve(this->columns_.size());
            for (auto & col : this->columns_) {
                if (!col->is_key_column()) {
                    column_names.push_back(col->get_label());
                }
            }
            return column_names;
        }
        template <class T> T get(unsigned long ridx, const std::string & col_name) {
            return this->row(ridx).get<T>(col_name);
        }
        template <class T> const T get(unsigned long ridx, const std::string & col_name) const {
            return this->row(ridx).get<T>(col_name);
        }
        template <class T> T get(unsigned long ridx, unsigned long cidx) {
            return this->row(ridx).get<T>(cidx);
        }
        template <class T> const T get(unsigned long ridx, unsigned long cidx) const {
            return this->row(ridx).get<T>(cidx);
        }
        template <class T>
        std::vector<T> get_column(const std::string & col_name) const {
            std::vector<T> vals;
            vals.reserve(this->rows_.size());
            for (auto & row : this->rows_) {
                vals.push_back(row->get<T>(col_name));
            }
            return vals;
        }
        template <class T>
        std::vector<T> get_column(unsigned long cidx) const {
            std::vector<T> vals;
            vals.reserve(this->rows_.size());
            for (auto & row : this->rows_) {
                vals.push_back(row->get<T>(cidx));
            }
            return vals;
        }

        template <class T=DataTableColumn::floating_point_implementation_type>
        Summary<T> summarize_column(const std::string & col_name) const {
            auto vals = this->get_column<T>(col_name);
            return DataTable::summarize(vals);
        }

        template <class T=DataTableColumn::floating_point_implementation_type>
        Summary<T> summarize_column(unsigned long cidx) const {
            auto vals = this->get_column<T>(cidx);
            return DataTable::summarize(vals);
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

        void write(std::ostream & out,
                const std::string & column_separator="\t",
                bool include_header_row=true) {

            if (include_header_row) {
                unsigned long printed_idx = 0;
                // TODO! Key columns may not all be given first!
                for (auto & col : this->columns_) {
                    if (!col->is_hidden()) {
                        if (printed_idx > 0) {
                            out << column_separator;
                        }
                        out << col->get_label();
                        printed_idx += 1;
                    }
                }
            }
            out << "\n";

            for (auto & row : this->rows_) {
                row->write_formatted(out, column_separator);
            }
        }

        void write_stacked(std::ostream & out,
                const std::string & stacked_field_identifier_label="key",
                const std::string & stacked_field_value_label="value",
                const std::string & column_separator="\t",
                bool include_header_row=true) {

            // sort key and data columns
            std::vector<Column *> key_columns;
            std::vector<Column *> data_columns;
            for (auto & col : this->columns_) {
                if (col->is_key_column()) {
                    key_columns.push_back(col);
                } else {
                    data_columns.push_back(col);
                }
            }

            // print headers
            unsigned long printed_idx = 0;
            if (include_header_row) {
                for (auto & col : this->columns_) {
                    if (col->is_key_column() && !col->is_hidden()) {
                        if (printed_idx > 0) {
                            out << column_separator;
                        }
                        out << col->get_label();
                        printed_idx += 1;
                    }
                }
            }
            if (printed_idx > 0) {
                out << column_separator;
            }
            out << stacked_field_identifier_label << column_separator << stacked_field_value_label << "\n";

            // print rows
            for (auto & row : this->rows_) {
                row->write_stacked(out,
                        key_columns,
                        data_columns,
                        column_separator);
            } // row
        }

    public:
        template <class T=DataTableColumn::floating_point_implementation_type>
        static Summary<T> summarize(const std::vector<T> & vals) {
            Summary<T> summary;
            summary.size = vals.size();
            if (summary.size > 0) {
                summary.sum = std::accumulate(vals.begin(), vals.end(), 0.0);
                summary.mean = summary.sum / summary.size;
                auto ss_f = [&summary](const T & a, const T & b)->T {
                    return a + std::pow(b - summary.mean, 2);
                };
                summary.sum_of_squares = std::accumulate(vals.begin(), vals.end(), 0.0, ss_f);
                if (summary.size > 1) {
                    summary.sample_variance = summary.sum_of_squares / (summary.size - 1);
                    summary.population_variance = summary.sum_of_squares / (summary.size);
                }
                auto range = std::minmax_element(vals.begin(), vals.end());
                summary.minimum = *(range.first);
                summary.maximum = *(range.second);
            }
            return summary;
        }

    private:
        template <class T> Column & create_column(
                const std::string & label,
                bool is_key_column=false,
                const platypus::stream::OutputStreamFormatters & formatters={}) {
            if (!this->rows_.empty()) {
                throw DataTableStructureError(__FILE__, __LINE__, "Cannot add new column: rows have already been added");
            }
            if (this->column_label_index_map_.find(label) != this->column_label_index_map_.end()) {
                throw DataTableStructureError(__FILE__, __LINE__, "Cannot add new column: duplicate column name");
            }
            auto new_col = new Column(*this, Column::identify_type<T>(), label, is_key_column, formatters);
            this->column_label_index_map_[label] = this->columns_.size();
            this->columns_.push_back(new_col);
            return *new_col;
        }
    private:
        std::vector<Column *>                   columns_;
        std::map<std::string, unsigned long>    column_label_index_map_;
        std::vector<Row *>                      rows_;
}; // DataTable

} // namespace platypus

//////////////////////////////////////////////////////////////////////////////
// DataTableColumn

template <class T> T platypus::DataTableColumn::max() const {
    std::vector<T> vals;
    vals.reserve(this->table_.num_rows());
    for (auto & row : this->table_) {
        vals.push_back(row.get<T>(this->label_));
    }
    return *(std::max_element(vals.begin(), vals.end()));
}

#endif
