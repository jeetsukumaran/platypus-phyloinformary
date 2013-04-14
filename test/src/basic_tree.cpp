#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "tests.hpp"

int main() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::ostringstream out;
    write_newick(tree, out);
    std::string result = out.str();
    trim(result, " \t\n\r");
    if (result == STANDARD_TEST_TREE_NEWICK) {
        exit(0);
    } else {
        fail_test(__FILE__, STANDARD_TEST_TREE_NEWICK, result);
    }
}
