#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::map<std::string, std::vector<std::string>> siblings;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        if (ndi == tree.begin()) {
            // skip root
            continue;
        }
        for (auto sib = tree.next_sibling_begin(ndi); sib != tree.next_sibling_end(); ++sib) {
            siblings[*ndi].push_back(*sib);
        }
        auto expected_iter = STANDARD_TEST_TREE_SIBLINGS.find(*ndi);
        assert(expected_iter != STANDARD_TEST_TREE_SIBLINGS.end());
        auto expected_siblings = expected_iter->second;
        if (siblings[*ndi] != expected_siblings) {
            int chk = test_equal(
                    expected_siblings,
                    siblings[*ndi],
                    __FILE__,
                    __LINE__,
                    "Start node: ", *ndi);
            if (chk == 0) {
                return EXIT_SUCCESS;
            } else {
                return EXIT_FAILURE;
            }
        }
    }
    return 0;
}
