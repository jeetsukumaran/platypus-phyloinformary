#include <cassert>
#include <cmath>
#include <cfloat>
#include <climits>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <functional>
#include <platypus/model/datatable.hpp>
#include "platypus_testing.hpp"

class DataTableTester {

    public:
        typedef platypus::DataTable::Column::signed_integer_implementation_type      signed_int_type;
        typedef platypus::DataTable::Column::unsigned_integer_implementation_type    unsigned_int_type;
        typedef platypus::DataTable::Column::floating_point_implementation_type      float_type;
        typedef platypus::DataTable::Column::string_implementation_type              str_type;

    public:

        //////////////////////////////////////////////////////////////////////////////
        // Lifecycle

        DataTableTester() {
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
        }

        //////////////////////////////////////////////////////////////////////////////
        // Tests

        int run_tests() {
            int fails = 0;
            fails += this->test_basic_data_table_construction();
            fails += this->test_iteration();
            fails += this->test_formatting_on_definition();
            fails += this->test_formatting_post_definition();
            return fails;
        }

        int test_basic_data_table_construction() {
            int fails = 0;
            platypus::DataTable table;
            this->build_data_table(table);
            fails += this->verify_data_table(table, false, false);
            fails += this->verify_data_table(table, false, true);
            return fails;
        }

        int test_iteration() {
            int fails = 0;
            platypus::DataTable table;
            this->build_data_table(table);

            unsigned long row_count = 0;
            unsigned long col_count = 0;
            for (auto & row : table) {
                col_count = 0;
                for (auto citer = row.begin<std::string>(); citer != row.end<std::string>(); ++ citer) {
                    // std::cout << *citer << std::endl;
                    // citer.set(0);
                    col_count += 1;
                }
                // std::cout << "--" << std::endl;
                fails += platypus::test::check_equal(
                        this->num_cols_,
                        col_count,
                        __FILE__,
                        __LINE__,
                        "default column iteration column count");
                row_count += 1;
            }
            fails += platypus::test::check_equal(
                    this->num_rows_,
                    row_count,
                    __FILE__,
                    __LINE__,
                    "default row iteration row count");

            fails += this->verify_data_table(table, false, true);
            return fails;
        }

        int test_formatting_on_definition() {
            int fails = 0;
            platypus::DataTable table;
            auto & col1 = table.add_data_column<float_type>("1", {std::setprecision(12)});
            auto & col2 = table.add_data_column<float_type>("2", {std::fixed, std::setprecision(2)});
            auto & col3 = table.add_data_column<float_type>("3", {std::scientific, std::setprecision(4)});
            table.new_row() << 22./7 << 22./7 << 22./7;
            std::vector<std::string> expected{"3.14285714286", "3.14", "3.1429e+00"};
            for (auto & row : table) {
                unsigned long col_idx = 0;
                for (auto & col : row) {
                    fails += platypus::test::check_equal(
                            expected.at(col_idx),
                            col,
                            __FILE__,
                            __LINE__,
                            "formatted output of column ", col_idx);
                    col_idx += 1;
                }
            }
            col1.set_formatting({std::fixed, std::setprecision(2)});
            col2.set_formatting({std::scientific, std::setprecision(2)});
            col3.set_formatting({std::setprecision(2)});
            expected = {"3.14", "3.14e+00", "3.1"};
            for (auto & row : table) {
                unsigned long col_idx = 0;
                for (auto & col : row) {
                    fails += platypus::test::check_equal(
                            expected.at(col_idx),
                            col,
                            __FILE__,
                            __LINE__,
                            "formatted output of column ", col_idx);
                    col_idx += 1;
                }
            }
            col1.clear_formatting();
            col2.clear_formatting();
            col3.clear_formatting();
            expected = {"3.14286", "3.14286", "3.14286"};
            for (auto & row : table) {
                unsigned long col_idx = 0;
                for (auto & col : row) {
                    fails += platypus::test::check_equal(
                            expected.at(col_idx),
                            col,
                            __FILE__,
                            __LINE__,
                            "formatted output of column ", col_idx);
                    col_idx += 1;
                }
            }
            return fails;
        }

        int test_formatting_post_definition() {
            int fails = 0;
            platypus::DataTable table;
            auto & col1 = table.add_data_column<float_type>("1");
            auto & col2 = table.add_data_column<float_type>("2");
            auto & col3 = table.add_data_column<float_type>("3");
            table.new_row() << 22./7 << 22./7 << 22./7;
            col1.add_formatting(std::setprecision(12));
            col2.add_formatting(std::fixed);
            col2.add_formatting(std::setprecision(2));
            col3.add_formatting(std::scientific);
            col3.add_formatting(std::setprecision(4));
            std::vector<std::string> expected{"3.14285714286", "3.14", "3.1429e+00"};
            for (auto & row : table) {
                unsigned long col_idx = 0;
                for (auto & col : row) {
                    fails += platypus::test::check_equal(
                            expected.at(col_idx),
                            col,
                            __FILE__,
                            __LINE__,
                            "formatted output of column ", col_idx);
                    col_idx += 1;
                }
            }
            return fails;
        }

        //////////////////////////////////////////////////////////////////////////////
        // Support

        void build_data_table(platypus::DataTable & table) {
            this->add_data_table_columns(table);
            this->populate_data_table(table);
        }

        void add_data_table_columns(platypus::DataTable & table) {
            for (unsigned idx = 0; idx < this->num_cols_; ++idx) {
                auto & label = this->labels_[idx];
                if (this->is_signed_int_col(idx)) {
                    if (this->is_key_col(idx)) {
                        table.add_key_column<signed_int_type>(label);
                    } else {
                        table.add_data_column<signed_int_type>(label);
                    }
                } else if (this->is_unsigned_int_col(idx)) {
                    if (this->is_key_col(idx)) {
                        table.add_key_column<unsigned_int_type>(label);
                    } else {
                        table.add_data_column<unsigned_int_type>(label);
                    }
                } else if (this->is_real_col(idx)) {
                    if (this->is_key_col(idx)) {
                        table.add_key_column<float_type>(label);
                    } else {
                        table.add_data_column<float_type>(label);
                    }
                } else {
                    if (this->is_key_col(idx)) {
                        table.add_key_column<str_type>(label);
                    } else {
                        table.add_data_column<str_type>(label);
                    }
                }
            }
        }

        void populate_data_table(platypus::DataTable & table) {
            for (unsigned ridx = 0; ridx < this->num_rows_; ++ridx) {
                auto & rec = table.new_row();
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
            if (table.num_rows() != this->num_rows_) {
                fails += platypus::test::fail_test(__FILE__, __LINE__, this->num_cols_, table.num_rows(), "number of rows");
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
                            "Row: ", ridx, ", Column: 0");
                    fails += platypus::test::check_equal(
                            this->col1_[ridx],
                            table.get<unsigned_int_type>(ridx, 1),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 1");
                    fails += platypus::test::check_equal(
                            this->col2_[ridx],
                            table.get<signed_int_type>(ridx, 2),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 2");
                    fails += platypus::test::check_equal(
                            this->col3_[ridx],
                            table.get<signed_int_type>(ridx, 3),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 3");
                    fails += platypus::test::check_equal(
                            this->col4_[ridx],
                            table.get<unsigned_int_type>(ridx, 4),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 4");
                    fails += platypus::test::check_equal(
                            this->col5_[ridx],
                            table.get<unsigned_int_type>(ridx, 5),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 5");
                    fails += platypus::test::check_almost_equal(
                            static_cast<float_type>(this->col6_[ridx]),
                            static_cast<float_type>(table.get<float_type>(ridx, 6)),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 6");
                    fails += platypus::test::check_almost_equal(
                            static_cast<float_type>(this->col7_[ridx]),
                            static_cast<float_type>(table.get<float_type>(ridx, 7)),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 7");
                    fails += platypus::test::check_equal(
                            this->col8_[ridx],
                            table.get<str_type>(ridx, 8),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 8");
                    fails += platypus::test::check_equal(
                            this->col9_[ridx],
                            table.get<str_type>(ridx, 9),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 9");
                }
            } else {
                for (unsigned ridx = 0; ridx < this->num_rows_; ++ridx) {
                    fails += platypus::test::check_equal(
                            this->col0_[ridx],
                            table.get<str_type>(ridx, 0),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 0");
                    fails += platypus::test::check_equal(
                            this->col1_[ridx],
                            table.get<unsigned_int_type>(ridx, 1),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 1");
                    fails += platypus::test::check_equal(
                            this->col2_[ridx],
                            table.get<signed_int_type>(ridx, 2),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 2");
                    fails += platypus::test::check_equal(
                            this->col3_[ridx],
                            table.get<signed_int_type>(ridx, 3),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 3");
                    fails += platypus::test::check_equal(
                            this->col4_[ridx],
                            table.get<unsigned_int_type>(ridx, 4),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 4");
                    fails += platypus::test::check_equal(
                            this->col5_[ridx],
                            table.get<unsigned_int_type>(ridx, 5),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 5");
                    fails += platypus::test::check_almost_equal(
                            static_cast<float_type>(this->col6_[ridx]),
                            static_cast<float_type>(table.get<float_type>(ridx, 6)),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 6");
                    fails += platypus::test::check_almost_equal(
                            static_cast<float_type>(this->col7_[ridx]),
                            static_cast<float_type>(table.get<float_type>(ridx, 7)),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 7");
                    fails += platypus::test::check_equal(
                            this->col8_[ridx],
                            table.get<str_type>(ridx, 8),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 8");
                    fails += platypus::test::check_equal(
                            this->col9_[ridx],
                            table.get<str_type>(ridx, 9),
                            __FILE__,
                            __LINE__,
                            "Row: ", ridx, ", Column: 9");
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
        std::vector<str_type>           col0_{"r0","r1","r2","r3","r4","r5","r6","r7","r8","r9","r10"};
        std::vector<unsigned_int_type>  col1_{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        std::vector<signed_int_type>    col2_{-5,-4,-3,-2,-1,0,1,2,3,4,5};
        std::vector<signed_int_type>    col3_{LONG_MAX, LONG_MIN, INT_MAX, INT_MIN, CHAR_MAX, CHAR_MIN, SHRT_MAX, SHRT_MIN, LONG_MAX/2, INT_MAX/2, 0};
        std::vector<unsigned_int_type>  col4_{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        std::vector<unsigned_int_type>  col5_{ULONG_MAX, ULONG_MAX/2, UINT_MAX, UINT_MAX/2, UCHAR_MAX, UCHAR_MAX/2, USHRT_MAX, USHRT_MAX/2, 9, 10, 11};
        std::vector<float_type>          col6_{ 22./7, 2.1782e-8/2.41e2, std::pow(3.412412e12, 2.14), 1.23213993e-8 * 412443./1.77e3, 93842848.681241221491924912491294192412491924912491294914292194921349192499992, 0.0000000000000000000000001294914292194921349192499992, -22./7, -2.1782e-8/2.41e2, -std::pow(3.412412e12, 2.14), -1.23213993e-8 * 412443./1.77e3, -0.0000000000000000000000001294914292194921349192499992};
        std::vector<float_type>          col7_{DBL_MAX, DBL_MIN, LDBL_MAX, LDBL_MIN, FLT_MAX, FLT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, LDBL_EPSILON, 0.0};
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
}; // DataTableTester

int main() {
    DataTableTester dtt;
    int fails = dtt.run_tests();
    if (fails > 0) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}
