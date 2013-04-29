#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include "tests.hpp"

using namespace platypus::test;

int main() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::map<std::string, std::vector<std::string>> children;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        for (auto chi = tree.children_begin(ndi); chi != tree.children_end(); ++chi) {
            children[*ndi].push_back(*chi);
        }
        auto expected_iter = STANDARD_TEST_TREE_CHILDREN.find(*ndi);
        assert(expected_iter != STANDARD_TEST_TREE_CHILDREN.end());
        auto expected_children = expected_iter->second;
        return check_equal(
                expected_children,
                children[*ndi],
                __FILE__,
                __LINE__,
                "parent node: ",
                *ndi);
    }
}
