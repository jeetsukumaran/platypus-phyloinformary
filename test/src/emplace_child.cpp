#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "platypus_testing.hpp"

using namespace platypus::test;

class Value {

    public:
        Value() { }
        Value(const char * v)
            : label_(v) { }

    private:
        std::string label_;

    friend std::ostream& operator<<(std::ostream& stream, const Value & v) ;
};

std::ostream& operator<<(std::ostream& stream, const Value & v) {
    stream << v.label_;
    return stream;
}

int main() {

    platypus::Tree<Value> tree;
    auto root = tree.begin();
    auto i1 = tree.emplace_child(root, "i1");
    auto i2 = tree.emplace_child(root, "i2");
    auto i3 = tree.emplace_child(i1, "i3");
    auto i4 = tree.emplace_child(i1, "i4");
    auto i5 = tree.emplace_child(i2, "i5");
    auto i6 = tree.emplace_child(i2, "i6");
    auto i7 = tree.emplace_child(i3, "i7");
    auto i8 = tree.emplace_child(i3, "i8");
    auto i9 = tree.emplace_child(i4, "i9");
    auto i10 = tree.emplace_child(i4, "i10");
    auto i11 = tree.emplace_child(i5, "i11");
    auto i12 = tree.emplace_child(i5, "i12");
    auto i13 = tree.emplace_child(i6, "i13");
    auto i14 = tree.emplace_child(i6, "i14");

    std::ostringstream out;
    write_newick(tree, out);
    std::string result = out.str();
    trim(result, " \t\n\r");
    std::string expected = "(((i7, i8)i3, (i9, i10)i4)i1, ((i11, i12)i5, (i13, i14)i6)i2);";
    int chk = test_equal( expected, result, __FILE__, __LINE__);
    if (chk == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
