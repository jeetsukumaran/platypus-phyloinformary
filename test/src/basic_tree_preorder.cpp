#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "tests.hpp"

int main() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::vector<std::string> visits;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        visits.push_back(*ndi);
    }
    if (visits == STANDARD_TEST_TREE_PREORDER) {
        exit(0);
    } else {
        fail_test(__FILE__,
            STANDARD_TEST_TREE_PREORDER,
            visits);
    }
}
