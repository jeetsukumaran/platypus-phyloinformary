#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "tests.hpp"

int main() {

    BasicTree tree_original;
    build_tree(tree_original, STANDARD_TEST_TREE_STRING);

    BasicTree tree_copy(tree_original);
    // tree_copy.head_node()->set_data("head");
    // tree_copy.stop_node()->set_data("stop");
    // tree_copy.deep_copy_from(tree_original);
    // tree_copy = tree_original;

    // Change labels are different to make sure that original tree
    // nodes are not linked to tree copy; and then check cloned tree
    // for validity
    for (auto ndi = tree_original.preorder_begin(); ndi != tree_original.preorder_end(); ++ndi) {
        std::string s = ndi->data();
        uppercase(s);
        *ndi = s;
    }
    check_newick(tree_copy, "cloned tree failed to yield expected newick string", STANDARD_TEST_TREE_NEWICK, false);
    check_newick(tree_original, "original tree yielded expected newick string even though labels modified", STANDARD_TEST_TREE_NEWICK, true);

    // Restore labels on original tree and make sure it is valid
    for (auto ndi = tree_original.preorder_begin(); ndi != tree_original.preorder_end(); ++ndi) {
        std::string s = ndi->data();
        lowercase(s);
        *ndi = s;
    }
    for (auto ndi = tree_copy.preorder_begin(); ndi != tree_copy.preorder_end(); ++ndi) {
        std::string s = ndi->data();
        uppercase(s);
        *ndi = s;
    }
    check_newick(tree_original, "original tree with restored labels failed to yield expected newick string", STANDARD_TEST_TREE_NEWICK, false);
    check_newick(tree_copy, "cloned tree yielded expected newick string even though labels modified", STANDARD_TEST_TREE_NEWICK, true);

    exit(0);
}
