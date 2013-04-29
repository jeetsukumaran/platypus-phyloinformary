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
    std::vector<std::string> visits;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        visits.push_back(*ndi);
    }
    return check_equal(
            STANDARD_TEST_TREE_PREORDER,
            visits,
            __FILE__,
            __LINE__);
}
