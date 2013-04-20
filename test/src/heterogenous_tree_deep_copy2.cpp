#include <stdlib.h>
#include <sstream>
#include <string>
#include <iostream>
#include "tests.hpp"

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
        Y () { }
        Y (const std::string & c)
            : label_(c) {
        }
        const Y & operator=(const X & x) {
            this->label_ = x.get_label();
            return *this;
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

std::ostream& operator<<(std::ostream& stream, const Y& data) {
    stream << data.get_label();
    return stream;
}

int main() {
    platypus::Tree<X> x;
    build_tree(x, STANDARD_TEST_TREE_STRING);
    platypus::Tree<Y> y;
    y.deep_copy_from(x);
    return check_newick(y, "cloned tree failed to yield expected newick string", STANDARD_TEST_TREE_NEWICK, false);
}

