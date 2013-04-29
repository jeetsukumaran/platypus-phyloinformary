#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::ostringstream out;
    write_newick(tree, out);
    std::string result = out.str();
    trim(result, " \t\n\r");
    return check_equal(STANDARD_TEST_TREE_NEWICK, result, __FILE__, __LINE__);
}
