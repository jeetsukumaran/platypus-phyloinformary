#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::vector<std::string> leaves;
    for (auto ndi = tree.leaf_begin(); ndi != tree.leaf_end(); ++ndi) {
        leaves.push_back(*ndi);
    }
    int chk = platypus::testing::compare_equal(
            STANDARD_TEST_TREE_LEAVES,
            leaves,
            __FILE__,
            __LINE__);
    if (chk == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
