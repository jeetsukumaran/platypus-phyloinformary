#include <cmath>
#include <cfloat>
#include <climits>
#include <vector>
#include <string>
#include <iostream>
#include <platypus/utility/datatable.hpp>

class TestReference {

    public:
        typedef platypus::Column::signed_integer_implementation_type    signed_int_type;
        typedef platypus::Column::unsigned_integer_implementation_type  unsigned_int_type;
        typedef platypus::Column::real_implementation_type              real_type;
        typedef platypus::Column::string_implementation_type            str_type;

    public:
        void build_data_table(platypus::DataTable & table) {
            this->define_data_table_columns(table);
        }

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
            return idx == 0 || idx == 8;
        }

        bool is_key_col(unsigned idx) {
            return idx <= 1;
        }

        void define_data_table_columns(platypus::DataTable & table) {
            for (unsigned idx = 0; idx < this->labels_.size(); ++idx) {
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


    private:
        std::vector<str_type>           labels_{"a","b","c","d","e","f","g","h","i","j","k"};
        std::vector<str_type>           col0{"a","b","c","d","e","f","g","h","i","j","k"};
        std::vector<unsigned_int_type>  col1{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        std::vector<signed_int_type>    col2{-5,-4,-3,-2,-1,0,1,2,3,4};
        std::vector<signed_int_type>    col3{LONG_MAX, LONG_MIN, INT_MAX, INT_MIN, CHAR_MAX, CHAR_MIN, SHRT_MAX, SHRT_MIN, LONG_MAX/2, INT_MAX/2, 0};
        std::vector<unsigned_int_type>  col4{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
        std::vector<unsigned_int_type>  col5{ULONG_MAX, ULONG_MAX/2, UINT_MAX, UINT_MAX/2, UCHAR_MAX, UCHAR_MAX/2, USHRT_MAX, USHRT_MAX/2, 9, 10, 11};
        std::vector<real_type>          col6{ 22./7, 2.1782e-8/2.41e2, std::pow(3.412412e12, 2.14), 1.23213993e-8 * 412443./1.77e3, 93842848.681241221491924912491294192412491924912491294914292194921349192499992, 0.0000000000000000000000001294914292194921349192499992, -22./7, -2.1782e-8/2.41e2, -std::pow(3.412412e12, 2.14), -1.23213993e-8 * 412443./1.77e3, -0.0000000000000000000000001294914292194921349192499992};
        std::vector<real_type>          col7{DBL_MAX, DBL_MIN, LDBL_MAX, LDBL_MIN, FLT_MAX, FLT_MIN, LONG_MAX, LONG_MIN, ULONG_MAX, LDBL_EPSILON, 0.0};
        std::vector<str_type>           col8{
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
}; // TestReference

int main() {
    TestReference ref;
    platypus::DataTable table;
    ref.build_data_table(table);
    table.dump();
}
