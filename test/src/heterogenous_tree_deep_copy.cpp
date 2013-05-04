#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>
#include "platypus_testing.hpp"

using namespace platypus::test;

class X {
    public:
        X () { }
        X (const std::string & c)
            : label_(c) {
        }
        const std::string & get_label() const {
            return this->label_;
        }
        void set_label(const std::string & s) {
            this->label_ = s;
        }
    private:
        std::string label_;
};

class Y {
    public:
        const Y & operator=(const X & x) {
            this->label_ = static_cast<int>(x.get_label()[0]) - 96;
            return *this;
        }
        const int get_label() const {
            return this->label_;
        }
        void set_label(const int s) {
            this->label_ = s;
        }
    private:
        int label_;
};

std::ostream& operator<<(std::ostream& stream, const Y& data) {
    stream << data.get_label();
    return stream;
}

int main() {
    platypus::Tree<X> x;
    build_tree(x, STANDARD_TEST_TREE_STRING);
    platypus::Tree<Y> y;
    y.deep_copy_from(x);
    // write_newick(y, std::cout);
    std::string expected = "((9, (10, 11)5)2, ((12, 13)7, (14, (15, 16)8)6)3)1;";
    int chk = compare_against_newick_string(y, "cloned tree failed to yield expected newick string", expected, false);
    if (chk == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

