#include <cassert>
#include <cmath>
#include <cfloat>
#include <climits>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <platypus/utility/datatable.hpp>
#include "platypus_testing.hpp"

class TestReference {

    public:
        typedef platypus::Column::signed_integer_implementation_type    signed_int_type;
        typedef platypus::Column::unsigned_integer_implementation_type  unsigned_int_type;
        typedef platypus::Column::real_implementation_type              real_type;
        typedef platypus::Column::string_implementation_type            str_type;

    public:

        TestReference(
                bool build_local_table=false,
                bool verify_local_table=false,
                bool exit_on_local_table_failure=false,
                bool verify_local_table_using_access_by_col_name=false) {
            this->num_cols_ = 10;
            this->num_rows_ = this->col0_.size();
            assert(this->col0_.size() == this->num_rows_);
            assert(this->col1_.size() == this->num_rows_);
            assert(this->col2_.size() == this->num_rows_);
            assert(this->col3_.size() == this->num_rows_);
            assert(this->col4_.size() == this->num_rows_);
            assert(this->col5_.size() == this->num_rows_);
            assert(this->col6_.size() == this->num_rows_);
            assert(this->col7_.size() == this->num_rows_);
            assert(this->col8_.size() == this->num_rows_);
            assert(this->col9_.size() == this->num_rows_);
            if (build_local_table) {
                this->build_data_table(this->reference_table_);
                if (verify_local_table) {
                    this->verify_data_table(this->reference_table_, exit_on_local_table_failure, verify_local_table_using_access_by_col_name);
                }
            }
        }

        void build_data_table(platypus::DataTable & table) {
            this->define_data_table_columns(table);
            this->populate_data_table(table);
        }

        void define_data_table_columns(platypus::DataTable & table) {
            for (unsigned idx = 0; idx < this->num_cols_; ++idx) {
                auto & label = this->labels_[idx];
                if (this->is_signed_int_col(idx)) {
                    if (this->is_key_col(idx)) {
                        table.define_key_column<signed_int_type>(label);
                    } else {
                        table.define_data_column<signed_int_type>(label);
                    }
                } else if (this->is_unsigned_int_col(idx)) {
                    if (this->is_key_col(idx)) {
                        table.define_key_column<unsigned_int_type>(label);
                    } else {
                        table.define_data_column<unsigned_int_type>(label);
                    }
                } else if (this->is_real_col(idx)) {
                    if (this->is_key_col(idx)) {
                        table.define_key_column<real_type>(label);
                    } else {
                        table.define_data_column<real_type>(label);
                    }
                } else {
                    if (this->is_key_col(idx)) {
                        table.define_key_column<str_type>(label);
                    } else {
                        table.define_data_column<str_type>(label);
                    }
                }
            }
        }

        void populate_data_table(platypus::DataTable & table) {
            for (unsigned ridx = 0; ridx < this->num_rows_; ++ridx) {
                auto & rec = table.new_record();
                rec << this->col0_[ridx];
                rec << this->col1_[ridx];
                rec << this->col2_[ridx];
                rec << this->col3_[ridx];
                rec << this->col4_[ridx];
                rec << this->col5_[ridx];
                rec << this->col6_[ridx];
                rec << this->col7_[ridx];
                rec << this->col8_[ridx];
                rec << this->col9_[ridx];
            }
        }


        int verify_data_table(platypus::DataTable & table,
                bool exit_on_fail=false,
                bool access_by_col_name=false) {
            int fails = 0;
            if (table.num_columns() != this->num_cols_) {
                fails += platypus::test::fail_test(__FILE__, __LINE__, this->num_cols_, table.num_columns(), "number of columns");
                if (exit_on_fail) {
                    exit(EXIT_FAILURE);
                }
            }
            if (table.num_records() != this->num_rows_) {
                fails += platypus::test::fail_test(__FILE__, __LINE__, this->num_cols_, table.num_records(), "number of records");
                if (exit_on_fail) {
                    exit(EXIT_FAILURE);
                }
            }
            if (access_by_col_name) {
                for (unsigned ridx = 0; ridx < this->num_rows_; ++ridx) {
                    fails += platypus::test::check_equal(
                            this->col0_[ridx],
                            table.get<str_type>(ridx, 0),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 0");
                    fails += platypus::test::check_equal(
                            this->col1_[ridx],
                            table.get<unsigned_int_type>(ridx, 1),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 1");
                    fails += platypus::test::check_equal(
                            this->col2_[ridx],
                            table.get<signed_int_type>(ridx, 2),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 2");
                    fails += platypus::test::check_equal(
                            this->col3_[ridx],
                            table.get<signed_int_type>(ridx, 3),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 3");
                    fails += platypus::test::check_equal(
                            this->col4_[ridx],
                            table.get<unsigned_int_type>(ridx, 4),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 4");
                    fails += platypus::test::check_equal(
                            this->col5_[ridx],
                            table.get<unsigned_int_type>(ridx, 5),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 5");
                    fails += platypus::test::check_almost_equal(
                            static_cast<real_type>(this->col6_[ridx]),
                            static_cast<real_type>(table.get<real_type>(ridx, 6)),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 6");
                    fails += platypus::test::check_almost_equal(
                            static_cast<real_type>(this->col7_[ridx]),
                            static_cast<real_type>(table.get<real_type>(ridx, 7)),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 7");
                    fails += platypus::test::check_equal(
                            this->col8_[ridx],
                            table.get<str_type>(ridx, 8),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 8");
                    fails += platypus::test::check_equal(
                            this->col9_[ridx],
                            table.get<str_type>(ridx, 9),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 9");
                }
            } else {
                for (unsigned ridx = 0; ridx < this->num_rows_; ++ridx) {
                    fails += platypus::test::check_equal(
                            this->col0_[ridx],
                            table.get<str_type>(ridx, 0),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 0");
                    fails += platypus::test::check_equal(
                            this->col1_[ridx],
                            table.get<unsigned_int_type>(ridx, 1),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 1");
                    fails += platypus::test::check_equal(
                            this->col2_[ridx],
                            table.get<signed_int_type>(ridx, 2),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 2");
                    fails += platypus::test::check_equal(
                            this->col3_[ridx],
                            table.get<signed_int_type>(ridx, 3),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 3");
                    fails += platypus::test::check_equal(
                            this->col4_[ridx],
                            table.get<unsigned_int_type>(ridx, 4),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 4");
                    fails += platypus::test::check_equal(
                            this->col5_[ridx],
                            table.get<unsigned_int_type>(ridx, 5),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 5");
                    fails += platypus::test::check_almost_equal(
                            static_cast<real_type>(this->col6_[ridx]),
                            static_cast<real_type>(table.get<real_type>(ridx, 6)),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 6");
                    fails += platypus::test::check_almost_equal(
                            static_cast<real_type>(this->col7_[ridx]),
                            static_cast<real_type>(table.get<real_type>(ridx, 7)),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 7");
                    fails += platypus::test::check_equal(
                            this->col8_[ridx],
                            table.get<str_type>(ridx, 8),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 8");
                    fails += platypus::test::check_equal(
                            this->col9_[ridx],
                            table.get<str_type>(ridx, 9),
                            __FILE__,
                            __LINE__,
                            "Record: ", ridx, ", Column: 9");
                }
            }
            return fails;
        }

    private:
        bool is_signed_int_col(unsigned idx) {
            return idx >= 2 && idx <= 3;
        }

        bool is_unsigned_int_col(unsigned idx) {
            return idx == 1 || (idx >= 3 && idx <=4);
        }

        bool is_real_col(unsigned idx) {
            return idx >= 6 && idx <= 7;
        }

        bool is_str_col(unsigned idx) {
            return idx == 0 || idx == 8 || idx == 9;
        }

        bool is_key_col(unsigned idx) {
            return idx <= 1;
        }

    private:
        unsigned long                   num_cols_;
        unsigned long                   num_rows_;
        std::vector<str_type>           labels_{"c0","c1","c2","c3","c4","c5","c6","c7","c8","c9","c10"};
        std::vector<str_type>           col0_{"a","b","c","d","e","f","g","h","i","j","k"};
        std::vector<unsigned_int_type>  col1_{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        std::vector<signed_int_type>    col2_{-5,-4,-3,-2,-1,0,1,2,3,4,5};
        std::vector<signed_int_type>    col3_{LONG_MAX, LONG_MIN, INT_MAX, INT_MIN, CHAR_MAX, CHAR_MIN, SHRT_MAX, SHRT_MIN, LONG_MAX/2, INT_MAX/2, 0};
        std::vector<unsigned_int_type>  col4_{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::vector<unsigned_int_type>  col5_{ULONG_MAX, ULONG_MAX/2, UINT_MAX, UINT_MAX/2, UCHAR_MAX, UCHAR_MAX/2, USHRT_MAX, USHRT_MAX/2, 9, 10, 11};
        std::vector<real_type>          col6_{ 22./7, 2.1782e-8/2.41e2, std::pow(3.412412e12, 2.14), 1.23213993e-8 * 412443./1.77e3, 93842848.681241221491924912491294192412491924912491294914292194921349192499992, 0.0000000000000000000000001294914292194921349192499992, -22./7, -2.1782e-8/2.41e2, -std::pow(3.412412e12, 2.14), -1.23213993e-8 * 412443./1.77e3, -0.0000000000000000000000001294914292194921349192499992};
        std::vector<real_type>          col7_{DBL_MAX, DBL_MIN, LDBL_MAX, LDBL_MIN, FLT_MAX, FLT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, LDBL_EPSILON, 0.0};
        std::vector<str_type>           col8_{
                "Philosophical Flying Fox",
                "Crazy Owl Monkey",
                "Pink Fairy Armadillo",
                "Perfectly Parachuting Peccary",
                "Stately Humpbacked Whale",
                "Frantic Duck-Billed Platypus",
                "Colorful Coral Snake",
                "Goofy Flemish Giant Bunny",
                "Grumpy Raccoon",
                "12345678910 11 12 13 14",
                "!@#$%%^^&**(())))))\n\t"};
        std::vector<const char *>     col9_{
                "the",
                "quick",
                "brown",
                "fox",
                "jumps over",
                "the lazy",
                "dog",
                "121.1 1",
                "&&2",
                "\n",
                ""};
        platypus::DataTable           reference_table_;
}; // TestReference

void validate_test_reference() {
    TestReference ref1(true, true, true, false);
    TestReference ref2(true, true, true, true);
}

int main() {
    validate_test_reference();
    int fails = 0;
    if (fails > 0) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}