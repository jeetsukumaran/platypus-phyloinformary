#include <stdlib.h>
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include "tests.hpp"

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
        if (children[*ndi] != expected_children) {
            return fail_test(__FILE__,
                expected_children,
                children[*ndi],
                "Parent node: ", *ndi);
        } else {
            return 0;
        }
    }
}
