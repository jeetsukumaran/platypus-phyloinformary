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
    for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
        visits.push_back(*ndi);
    }
    return check_equal(
            STANDARD_TEST_TREE_POSTORDER,
            visits,
            __FILE__,
            __LINE__);
}
