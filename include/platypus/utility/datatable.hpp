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

namespace platypus {

namespace datatable {

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
// Column

class Column {
    public:
        enum class ValueType {
                SignedInteger,
                UnsignedInteger,
                Real,
                String };
        typedef long             signed_integer_implementation_type;
        typedef unsigned long    unsigned_integer_implementation_type;
        typedef long double      real_implementation_type;
        typedef std::string      string_implementation_type;
        template <class T> static ValueType identify_type() {
            if (std::is_floating_point<T>::value) {
                return ValueType::Real;
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
        Column(ValueType value_type,
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
        template <class T> void format(std::ostream & out, const T & val) const {
            for (auto m : this->format_manipulators_) {
                out << m;
            }
            out << val;
        }
    protected:
        ValueType                   value_type_;
        std::string                 label_;
        OutputStreamManipulators    format_manipulators_;
}; // Column

//////////////////////////////////////////////////////////////////////////////
// BaseCell

class BaseCell {
    public:
        BaseCell(const Column & column)
            : column_(column) { }
        virtual ~BaseCell() { }
        const Column & get_column() const {
            return this->column_;
        }
    protected:
        const Column &    column_;
}; // BaseCell

//////////////////////////////////////////////////////////////////////////////
// TypedCell

template <class T>
class TypedCell : public BaseCell {
    public:
        TypedCell(const Column & column)
            : BaseCell(column) { }
        TypedCell(const Column & column, const T & val)
            : BaseCell(column)
            , value_(val) { }
        virtual ~TypedCell() {}
        template <class U> void set(const U & val) {
            std::ostringstream o;
            o << std::setprecision(64) << val;
            std::istringstream i(o.str());
            i >> std::setprecision(64) >> this->value_;
        }
        template <class U> U get() const {
            std::ostringstream o;
            this->column_.format(o, this->value_);
            std::istringstream i(o.str());
            U u;
            i >> u;
            return u;
        }
    protected:
        T       value_;
}; // BaseCell

class SignedIntegerCell : public TypedCell<Column::signed_integer_implementation_type> {
    public:
        typedef Column::signed_integer_implementation_type value_implementation_type;
        SignedIntegerCell(const Column & column)
            : TypedCell<value_implementation_type>(column, 0) { }
        template <class U> void set(const U & val) {
            TypedCell<value_implementation_type>::set(val);
        }
        template <class U> U get() const {
            return TypedCell<value_implementation_type>::get<U>();
        }
        void set(value_implementation_type val) {
            this->value_ = val;
        }
}; // specialization for long
template<> void SignedIntegerCell::set(const short int & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const unsigned short int & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const int & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const unsigned int & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const long int & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const unsigned long int & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const long long int & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const unsigned long long int & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const float & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const double & val) { this->value_ = val; }
template<> void SignedIntegerCell::set(const long double & val) { this->value_ = val; }
template<> short int SignedIntegerCell::get() const { return static_cast<short int>(this->value_); }
template<> unsigned short int SignedIntegerCell::get() const { return static_cast<unsigned short int>(this->value_); }
template<> int SignedIntegerCell::get() const { return static_cast<int>(this->value_); }
template<> unsigned int SignedIntegerCell::get() const { return static_cast<unsigned int>(this->value_); }
template<> long int SignedIntegerCell::get() const { return static_cast<long int>(this->value_); }
template<> unsigned long int SignedIntegerCell::get() const { return static_cast<unsigned long int>(this->value_); }
template<> long long int SignedIntegerCell::get() const { return static_cast<long long int>(this->value_); }
template<> unsigned long long int SignedIntegerCell::get() const { return static_cast<unsigned long long int>(this->value_); }
template<> float SignedIntegerCell::get() const { return static_cast<float>(this->value_); }
template<> double SignedIntegerCell::get() const { return static_cast<double>(this->value_); }
template<> long double SignedIntegerCell::get() const { return static_cast<long double>(this->value_); }

class UnsignedIntegerCell : public TypedCell<Column::unsigned_integer_implementation_type> {
    public:
        typedef Column::unsigned_integer_implementation_type value_implementation_type;
        UnsignedIntegerCell(const Column & column)
            : TypedCell<value_implementation_type>(column, 0) { }
        template <class U> void set(const U & val) {
            TypedCell<value_implementation_type>::set(val);
        }
        template <class U> U get() const {
            return TypedCell<value_implementation_type>::get<U>();
        }
}; // specialization for unsigned long
template<> void UnsignedIntegerCell::set(const short int & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const unsigned short int & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const int & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const unsigned int & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const long int & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const unsigned long int & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const long long int & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const unsigned long long int & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const float & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const double & val) { this->value_ = val; }
template<> void UnsignedIntegerCell::set(const long double & val) { this->value_ = val; }
template<> short int UnsignedIntegerCell::get() const { return static_cast<short int>(this->value_); }
template<> unsigned short int UnsignedIntegerCell::get() const { return static_cast<unsigned short int>(this->value_); }
template<> int UnsignedIntegerCell::get() const { return static_cast<int>(this->value_); }
template<> unsigned int UnsignedIntegerCell::get() const { return static_cast<unsigned int>(this->value_); }
template<> long int UnsignedIntegerCell::get() const { return static_cast<long int>(this->value_); }
template<> unsigned long int UnsignedIntegerCell::get() const { return static_cast<unsigned long int>(this->value_); }
template<> long long int UnsignedIntegerCell::get() const { return static_cast<long long int>(this->value_); }
template<> unsigned long long int UnsignedIntegerCell::get() const { return static_cast<unsigned long long int>(this->value_); }
template<> float UnsignedIntegerCell::get() const { return static_cast<float>(this->value_); }
template<> double UnsignedIntegerCell::get() const { return static_cast<double>(this->value_); }
template<> long double UnsignedIntegerCell::get() const { return static_cast<long double>(this->value_); }

class RealCell : public TypedCell<Column::real_implementation_type> {
    public:
        typedef Column::real_implementation_type value_implementation_type;
        RealCell(const Column & column)
            : TypedCell<value_implementation_type>(column, 0.0) { }
        template <class U> void set(const U & val) {
            TypedCell<value_implementation_type>::set(val);
        }
        template <class U> U get() const {
            return TypedCell<value_implementation_type>::get<U>();
        }
}; // specialization for double
template<> void RealCell::set(const short int & val) { this->value_ = val; }
template<> void RealCell::set(const unsigned short int & val) { this->value_ = val; }
template<> void RealCell::set(const int & val) { this->value_ = val; }
template<> void RealCell::set(const unsigned int & val) { this->value_ = val; }
template<> void RealCell::set(const long int & val) { this->value_ = val; }
template<> void RealCell::set(const unsigned long int & val) { this->value_ = val; }
template<> void RealCell::set(const long long int & val) { this->value_ = val; }
template<> void RealCell::set(const unsigned long long int & val) { this->value_ = val; }
template<> void RealCell::set(const float & val) { this->value_ = val; }
template<> void RealCell::set(const double & val) { this->value_ = val; }
template<> void RealCell::set(const long double & val) { this->value_ = val; }
template<> short int RealCell::get() const { return static_cast<short int>(this->value_); }
template<> unsigned short int RealCell::get() const { return static_cast<unsigned short int>(this->value_); }
template<> int RealCell::get() const { return static_cast<int>(this->value_); }
template<> unsigned int RealCell::get() const { return static_cast<unsigned int>(this->value_); }
template<> long int RealCell::get() const { return static_cast<long int>(this->value_); }
template<> unsigned long int RealCell::get() const { return static_cast<unsigned long int>(this->value_); }
template<> long long int RealCell::get() const { return static_cast<long long int>(this->value_); }
template<> unsigned long long int RealCell::get() const { return static_cast<unsigned long long int>(this->value_); }
template<> float RealCell::get() const { return static_cast<float>(this->value_); }
template<> double RealCell::get() const { return static_cast<double>(this->value_); }
template<> long double RealCell::get() const { return static_cast<long double>(this->value_); }

class StringCell : public TypedCell<Column::string_implementation_type> {
    public:
        typedef Column::string_implementation_type value_implementation_type;
        StringCell(const Column & column)
            : TypedCell<value_implementation_type>(column) { }
        template <class U> void set(const U & val) {
            TypedCell<value_implementation_type>::set(val);
        }
        template <class U> U get() const {
            return TypedCell<value_implementation_type>::get<U>();
        }
        void set(const char * val) {
            this->value_ = val;
        }
        void set(const value_implementation_type & val) {
            this->value_ = val;
        }
}; // specialization for std::string
template<> std::string StringCell::get() const { return this->value_; }

//////////////////////////////////////////////////////////////////////////////
// Row

class Row {

    public:
        Row(std::vector<Column *> & key_columns, std::vector<Column *> & data_columns)
                : key_columns_(key_columns)
                , data_columns_(data_columns)
                , current_entry_cell_idx_(0) {
            this->key_columns_size_ = this->key_columns_.size();
            this->data_columns_size_ = this->data_columns_.size();
            this->create_cells();
        }

        template <class T>
        const T get(unsigned long column_idx) const {
            auto * base_cell = this->cells_.at(column_idx);
            return Row::get_cell_value<T>(base_cell);
        }

        template <class T> T get(const std::string & col_name) const {
            auto citer = this->column_label_cell_map_.find(col_name);
            if (citer == this->column_label_cell_map_.end()) {
                throw std::runtime_error("Unrecognized column name");
            }
            auto * cell = citer->second;
            return Row::get_cell_value<T>(cell);
        }

        template <class T>
        Row & operator<<(const T & val) {
            auto * cell = this->cells_.at(this->current_entry_cell_idx_);
            this->set_cell_value(cell, val);
            ++this->current_entry_cell_idx_;
            return * this;
        }

        template <class T>
        void set(unsigned long column_idx, const T & val) {
            auto * cell = this->cells_.at(column_idx);
            this->set_cell_value(cell, val);
        }

        template <class T>
        void set(const std::string & col_name, const T & val) {
            auto citer = this->column_label_cell_map_.find(col_name);
            if (citer == this->column_label_cell_map_.end()) {
                throw std::runtime_error("Unrecognized column name");
            }
            auto * cell = citer->second;
            this->set_cell_value(cell, val);
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
                        this->current_value_ = Row::get_cell_value<ValueT>(*this->cell_iter_);
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
                            this->current_value_ = Row::get_cell_value<ValueT>(*this->cell_iter_);
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
                        Row::set_cell_value(*this->cell_iter_, val);
                        this->current_value_ = Row::get_cell_value<ValueT>(*this->cell_iter_);
                    } else {
                        throw std::runtime_error("Invalid iterator");
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

        void write_row(
                std::ostream & out,
                std::string separator="\t") {
            unsigned long column_idx = 0;
            for (auto & val : *this) {
                if (column_idx > 0) {
                    out << separator;
                }
                out << val;
                ++column_idx;
            }
            out << std::endl;
        }

    public:
        template <class T>
        static const T get_cell_value(BaseCell * cell) {
            auto & column_definition = cell->get_column();
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == Column::ValueType::SignedInteger) {
                SignedIntegerCell * t = dynamic_cast<SignedIntegerCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == Column::ValueType::UnsignedInteger) {
                UnsignedIntegerCell * t = dynamic_cast<UnsignedIntegerCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == Column::ValueType::Real) {
                RealCell * t = dynamic_cast<RealCell *>(cell);
                return t->get<T>();
            } else if (cell_value_type == Column::ValueType::String) {
                StringCell * t = dynamic_cast<StringCell *>(cell);
                return t->get<T>();
            } else {
                throw std::runtime_error("Unrecognized column type");
            }
        }

        template <class T>
        static void set_cell_value(BaseCell * cell, const T & val) {
            auto & column_definition = cell->get_column();
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == Column::ValueType::SignedInteger) {
                SignedIntegerCell * t = dynamic_cast<SignedIntegerCell *>(cell);
                t->set(val);
            } else if (cell_value_type == Column::ValueType::UnsignedInteger) {
                UnsignedIntegerCell * t = dynamic_cast<UnsignedIntegerCell *>(cell);
                t->set(val);
            } else if (cell_value_type == Column::ValueType::Real) {
                RealCell * t = dynamic_cast<RealCell *>(cell);
                t->set(val);
            } else if (cell_value_type == Column::ValueType::String) {
                StringCell * t = dynamic_cast<StringCell *>(cell);
                t->set(val);
            } else {
                throw std::runtime_error("Unrecognized column type");
            }
        }

    protected:
        void create_cells() {
            unsigned long total_columns_size = this->key_columns_.size() + this->data_columns_.size();
            this->cells_.reserve(total_columns_size);
            for (auto & key_column : this->key_columns_) {
                this->insert_cell(key_column);
            }
            for (auto & data_column : this->data_columns_) {
                this->insert_cell(data_column);
            }
        }

        void insert_cell(Column * column) {
            auto cell_value_type = column->get_value_type();
            BaseCell * new_cell = nullptr;
            if (cell_value_type == Column::ValueType::SignedInteger) {
                new_cell = new SignedIntegerCell(*column);
            } else if (cell_value_type == Column::ValueType::UnsignedInteger) {
                new_cell = new UnsignedIntegerCell(*column);
            } else if (cell_value_type == Column::ValueType::Real) {
                new_cell = new RealCell(*column);
            } else if (cell_value_type == Column::ValueType::String) {
                new_cell = new StringCell(*column);
            } else {
                throw std::runtime_error("Unrecognized column type");
            }
            this->column_label_cell_map_[column->get_label()] = new_cell;
            this->cells_.push_back(new_cell);
        }

    private:
        std::vector<Column *> &              key_columns_;
        std::vector<Column *> &              data_columns_;
        std::vector<BaseCell *>              cells_;
        unsigned long                        current_entry_cell_idx_;
        unsigned long                        key_columns_size_;
        unsigned long                        data_columns_size_;
        std::map<std::string, BaseCell *>    column_label_cell_map_;

}; // Row

} // namespace datatable

//////////////////////////////////////////////////////////////////////////////
// Table

class DataTable {

    public:
        typedef datatable::Column                      Column;
        typedef datatable::Row                         Row;
        typedef datatable::OutputStreamManipulator     OutputStreamManipulator;
        typedef datatable::OutputStreamManipulators    OutputStreamManipulators;

    public:
        DataTable()
            : is_rows_added_(false) {
        }
        ~DataTable() {
            for (auto & c : this->key_columns_) {
                delete c;
            }
            for (auto & c : this->data_columns_) {
                delete c;
            }
        }
        // template <class T> Column & define_key_column(const std::string & label) {
        //     return this->add_column<T>(this->key_columns_, label);
        // }
        // template <class T> Column & define_data_column(const std::string & label) {
        //     return this->add_column<T>(this->data_columns_, label);
        // }
        template <class T> Column & define_key_column(const std::string & label, const OutputStreamManipulators & format_manipulators={}) {
            return this->add_column<T>(this->key_columns_, label, format_manipulators);
        }
        template <class T> Column & define_data_column(const std::string & label, const OutputStreamManipulators & format_manipulators={}) {
            return this->add_column<T>(this->data_columns_, label, format_manipulators);
        }
        Row & new_row() {
            this->rows_.emplace_back(this->key_columns_, this->data_columns_);
            return this->rows_.back();
            this->is_rows_added_ = true;
        }
        void dump() {
            this->write_header_row(std::cout);
            for (auto & row : this->rows_) {
                row.write_row(std::cout);
            }
        }
        void dump_column(unsigned long column_idx) {
            for (auto & row : this->rows_) {
                std::cout << row.get<std::string>(column_idx) << ", ";
            }
            std::cout << std::endl;
        }
        unsigned long num_columns() const {
            return this->column_names_.size();
        }
        unsigned long num_rows() const {
            return this->rows_.size();
        }
        Row & operator[](unsigned long ridx) {
            return this->rows_.at(ridx);
        }
        template <class T> T get(unsigned long ridx, const std::string & col_name) {
            return this->rows_.at(ridx).get<T>(col_name);
        }
        template <class T> T get(unsigned long ridx, unsigned long fidx) {
            return this->rows_.at(ridx).get<T>(fidx);
        }
        void write_header_row(std::ostream & out, std::string separator="\t") {
            unsigned long column_idx = 0;
            for (auto & col_ptr : this->key_columns_) {
                auto key_column = *col_ptr;
                if (column_idx > 0) {
                    out << separator;
                }
                out << key_column.get_label();
                ++column_idx;
            }
            for (auto & col_ptr : this->data_columns_) {
                auto data_column = *col_ptr;
                if (column_idx > 0) {
                    out << separator;
                }
                out << data_column.get_label();
                ++column_idx;
            }
            out << std::endl;
        }

        /////////////////////////////////////////////////////////////////////////
        // Iterators
        typedef std::vector<Row>::iterator               iterator;
        typedef std::vector<Row>::const_iterator         const_iterator;
        typedef std::vector<Row>::reverse_iterator       reverse_iterator;
        typedef std::vector<Row>::const_reverse_iterator const_reverse_iterator;
        iterator begin() {
            return this->rows_.begin();
        }
        iterator end() {
            return this->rows_.end();
        }
        const_iterator begin() const {
            return this->rows_.begin();
        }
        const_iterator end() const {
            return this->rows_.end();
        }
        const_iterator cbegin() const {
            return this->rows_.cbegin();
        }
        const_iterator cend() const {
            return this->rows_.cend();
        }
        reverse_iterator rbegin() {
            return this->rows_.rbegin();
        }
        reverse_iterator rend() {
            return this->rows_.rend();
        }
        const_reverse_iterator rbegin() const {
            return this->rows_.rbegin();
        }
        const_reverse_iterator rend() const {
            return this->rows_.rend();
        }

    private:
        template <class T> Column & add_column(
                std::vector<Column *> & columns,
                const std::string & label,
                const OutputStreamManipulators & format_manipulators={}) {
            if (this->is_rows_added_) {
                throw std::runtime_error("Cannot add new column: rows have already been added");
            }
            if (this->column_names_.find(label) != this->column_names_.end()) {
                throw std::runtime_error("Cannot add new column: duplicate column name");
            }
            this->column_names_.insert(label);
            auto new_col = new Column(Column::identify_type<T>(), label, format_manipulators);
            columns.push_back(new_col);
            return *new_col;
        }
    private:
        std::vector<Column *>    key_columns_;
        std::vector<Column *>    data_columns_;
        std::vector<Row>         rows_;
        bool                     is_rows_added_;
        std::set<std::string>    column_names_;
}; // DataTable

} // namespace platypus

#endif
