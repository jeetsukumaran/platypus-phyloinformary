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
    for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
        visits.push_back(*ndi);
    }
    if (visits == STANDARD_TEST_TREE_POSTORDER) {
        exit(0);
    } else {
        fail_test(__FILE__,
            STANDARD_TEST_TREE_POSTORDER,
            visits);
    }
}
