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

#include <map>
#include <set>
#include <sstream>
#include <typeinfo>
#include <stdexcept>
#include <string>
#include <vector>
#include <iostream>

namespace platypus {

//////////////////////////////////////////////////////////////////////////////
// Column

class Column {
    public:
        enum class ValueType { SignedInteger, UnsignedInteger, Real, String };
        typedef long            signed_integer_implementation_type;
        typedef unsigned long unsigned_integer_implementation_type;
        typedef long double               real_implementation_type;
        typedef std::string             string_implementation_type;
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
        Column(ValueType value_type, const std::string & label)
                : value_type_(value_type)
                , label_(label) {
        }
        const std::string & get_label() const {
            return this->label_;
        }
        ValueType get_value_type() const {
            return this->value_type_;
        }
    protected:
        ValueType       value_type_;
        std::string     label_;
}; // Column

//////////////////////////////////////////////////////////////////////////////
// BaseCell

class BaseCell {
    public:
        BaseCell() { }
        virtual ~BaseCell() { }
}; // BaseCell

//////////////////////////////////////////////////////////////////////////////
// TypedCell

template <class T>
class TypedCell : public BaseCell {
    public:
        virtual ~TypedCell() {}
        template <class U> void set(const U & val) {
            std::ostringstream o;
            o << val;
            std::istringstream i(o.str());
            i >> this->value_;
        }
        template <class U> U get() const {
            std::ostringstream o;
            o << this->value_;
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
template<> long double SignedIntegerCell::get() const { return static_cast<double>(this->value_); }

class UnsignedIntegerCell : public TypedCell<Column::unsigned_integer_implementation_type> {
    public:
        typedef Column::unsigned_integer_implementation_type value_implementation_type;
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
template<> long double UnsignedIntegerCell::get() const { return static_cast<double>(this->value_); }

class RealCell : public TypedCell<Column::real_implementation_type> {
    public:
        typedef Column::real_implementation_type value_implementation_type;
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
template<> long double RealCell::get() const { return static_cast<double>(this->value_); }

class StringCell : public TypedCell<Column::string_implementation_type> {
    public:
        typedef Column::string_implementation_type value_implementation_type;
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
// Record

class Record {

    public:
        Record(std::vector<Column> & key_columns, std::vector<Column> & data_columns)
                : key_columns_(key_columns)
                , data_columns_(data_columns)
                , current_entry_cell_idx_(0) {
            this->key_columns_size_ = this->key_columns_.size();
            this->data_columns_size_ = this->data_columns_.size();
            this->total_columns_size_ = this->key_columns_size_ + this->data_columns_size_;
            this->create_cells();
        }

        template <class T>
        Record & operator<<(const T & val) {
            auto & column_definition = *(this->aggregated_columns_.at(this->current_entry_cell_idx_));
            auto * base_cell = this->cells_[this->current_entry_cell_idx_];
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == Column::ValueType::SignedInteger) {
                SignedIntegerCell * t = dynamic_cast<SignedIntegerCell *>(base_cell);
                t->set(val);
            } else if (cell_value_type == Column::ValueType::UnsignedInteger) {
                UnsignedIntegerCell * t = dynamic_cast<UnsignedIntegerCell *>(base_cell);
                t->set(val);
            } else if (cell_value_type == Column::ValueType::Real) {
                RealCell * t = dynamic_cast<RealCell *>(base_cell);
                t->set(val);
            } else if (cell_value_type == Column::ValueType::String) {
                StringCell * t = dynamic_cast<StringCell *>(base_cell);
                t->set(val);
            } else {
                throw std::runtime_error("Unrecognized column type");
            }
            ++this->current_entry_cell_idx_;
            return * this;
        }

        template <class T>
        const T get(unsigned long column_idx) {
            auto & column_definition = *(this->aggregated_columns_.at(column_idx));
            auto * base_cell = this->cells_[column_idx];
            auto cell_value_type = column_definition.get_value_type();
            if (cell_value_type == Column::ValueType::SignedInteger) {
                SignedIntegerCell * t = dynamic_cast<SignedIntegerCell *>(base_cell);
                return t->get<T>();
            } else if (cell_value_type == Column::ValueType::UnsignedInteger) {
                UnsignedIntegerCell * t = dynamic_cast<UnsignedIntegerCell *>(base_cell);
                return t->get<T>();
            } else if (cell_value_type == Column::ValueType::Real) {
                RealCell * t = dynamic_cast<RealCell *>(base_cell);
                return t->get<T>();
            } else if (cell_value_type == Column::ValueType::String) {
                StringCell * t = dynamic_cast<StringCell *>(base_cell);
                return t->get<T>();
            } else {
                throw std::runtime_error("Unrecognized column type");
            }
        }

        void write_row(
                std::ostream & out,
                std::string separator="\t") {
            unsigned long column_idx = 0;
            for (auto * base_cell : this->cells_) {
                auto & column_definition = *(this->aggregated_columns_.at(column_idx));
                auto cell_value_type = column_definition.get_value_type();
                if (column_idx > 0) {
                    out << separator;
                }
                if (cell_value_type == Column::ValueType::SignedInteger) {
                    SignedIntegerCell * t = dynamic_cast<SignedIntegerCell *>(base_cell);
                    out << t->get<SignedIntegerCell::value_implementation_type>();
                } else if (cell_value_type == Column::ValueType::UnsignedInteger) {
                    UnsignedIntegerCell * t = dynamic_cast<UnsignedIntegerCell *>(base_cell);
                    out << t->get<UnsignedIntegerCell::value_implementation_type>();
                } else if (cell_value_type == Column::ValueType::Real) {
                    RealCell * t = dynamic_cast<RealCell *>(base_cell);
                    out << t->get<RealCell::value_implementation_type>();
                } else if (cell_value_type == Column::ValueType::String) {
                    StringCell * t = dynamic_cast<StringCell *>(base_cell);
                    out << t->get<StringCell::value_implementation_type>();
                } else {
                    throw std::runtime_error("Unrecognized column type");
                }
                ++column_idx;
            }
            out << std::endl;
        }

    private:
        void create_cells() {
            this->cells_.reserve(this->total_columns_size_);
            this->aggregated_columns_.reserve(this->total_columns_size_);
            for (auto & key_column : this->key_columns_) {
                this->insert_cell(key_column);
            }
            for (auto & data_column : this->data_columns_) {
                this->insert_cell(data_column);
            }
        }

        void insert_cell(Column & column) {
            auto cell_value_type = column.get_value_type();
            BaseCell * new_cell = nullptr;
            if (cell_value_type == Column::ValueType::SignedInteger) {
                new_cell = new SignedIntegerCell();
            } else if (cell_value_type == Column::ValueType::UnsignedInteger) {
                new_cell = new UnsignedIntegerCell();
            } else if (cell_value_type == Column::ValueType::Real) {
                new_cell = new RealCell();
            } else if (cell_value_type == Column::ValueType::String) {
                new_cell = new StringCell();
            } else {
                throw std::runtime_error("Unrecognized column type");
            }
            this->column_label_cell_map_[column.get_label()] = new_cell;
            this->column_label_definition_map_[column.get_label()] = &column;
            this->cells_.push_back(new_cell);
            this->aggregated_columns_.push_back(&column);
        }

    private:
        std::vector<Column> &              key_columns_;
        std::vector<Column> &              data_columns_;
        std::vector<BaseCell *>                     cells_;
        unsigned long                               current_entry_cell_idx_;
        unsigned long                               key_columns_size_;
        unsigned long                               data_columns_size_;
        unsigned long                               total_columns_size_;
        std::map<std::string, BaseCell *>           column_label_cell_map_;
        std::map<std::string, Column *>    column_label_definition_map_;
        std::vector<Column *>              aggregated_columns_;

}; // Record

//////////////////////////////////////////////////////////////////////////////
// Table

class DataTable {
    public:
        DataTable()
            : records_added_(false) {
        }
        template <class T> void define_key_column(const std::string & label) {
            this->add_column<T>(this->key_columns_, label);
        }
        template <class T> void define_data_column(const std::string & label) {
            this->add_column<T>(this->data_columns_, label);
        }
        Record & new_record() {
            this->records_.emplace_back(this->key_columns_, this->data_columns_);
            return this->records_.back();
            this->records_added_ = true;
        }
        void dump() {
            this->write_header_row(std::cout);
            for (auto & record : this->records_) {
                record.write_row(std::cout);
            }
        }
        void dump_column(unsigned long column_idx) {
            for (auto & record : this->records_) {
                std::cout << record.get<std::string>(column_idx) << ", ";
            }
            std::cout << std::endl;
        }
        Record & operator[](unsigned long ridx) {
            return this->records_.at(ridx);
        }
        template <class T> T get(unsigned long ridx, unsigned long fidx) {
            return this->records_.at(ridx).get<T>(fidx);
        }
        void write_header_row(std::ostream & out, std::string separator="\t") {
            unsigned long column_idx = 0;
            for (auto & key_column : this->key_columns_) {
                if (column_idx > 0) {
                    out << separator;
                }
                out << key_column.get_label();
                ++column_idx;
            }
            for (auto & data_column : this->data_columns_) {
                if (column_idx > 0) {
                    out << separator;
                }
                out << data_column.get_label();
                ++column_idx;
            }
            out << std::endl;
        }

    private:
        template <class T> void add_column(std::vector<Column> & columns, const std::string & label) {
            if (this->records_added_) {
                throw std::runtime_error("Cannot add new column: records have already been added");
            }
            if (this->column_names_.find(label) != this->column_names_.end()) {
                throw std::runtime_error("Cannot add new column: duplicate column name");
            }
            columns.emplace_back(Column::identify_type<T>(), label);
        }
    private:
        std::vector<Column>   key_columns_;
        std::vector<Column>   data_columns_;
        std::vector<Record>   records_;
        bool                  records_added_;
        std::set<std::string> column_names_;
}; // DataTable

} // namespace platypus

#endif
