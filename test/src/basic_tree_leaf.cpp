#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "tests.hpp"

int main() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::vector<std::string> leaves;
    for (auto ndi = tree.leaf_begin(); ndi != tree.leaf_end(); ++ndi) {
        leaves.push_back(*ndi);
    }
    if (leaves == STANDARD_TEST_TREE_LEAVES) {
        exit(0);
    } else {
        fail_test(__FILE__,
            STANDARD_TEST_TREE_LEAVES,
            leaves);
    }
}
