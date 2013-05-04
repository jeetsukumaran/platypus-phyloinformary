#include <stdlib.h>
#include <sstream>
#include <iostream>
#include "platypus_testing.hpp"

using namespace platypus::test;

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
        std::string s = *ndi;
        uppercase(s);
        *ndi = s;
    }
    if (compare_against_newick_string(tree_copy, "cloned tree failed to yield expected newick string", STANDARD_TEST_TREE_NEWICK, false)) {
        return EXIT_FAILURE;
    };
    if (compare_against_newick_string(tree_original, "original tree yielded expected newick string even though labels modified", STANDARD_TEST_TREE_NEWICK, true)) {
        return EXIT_FAILURE;
    }
    // Restore labels on original tree and make sure it is valid
    for (auto ndi = tree_original.preorder_begin(); ndi != tree_original.preorder_end(); ++ndi) {
        std::string s = *ndi;
        lowercase(s);
        *ndi = s;
    }
    for (auto ndi = tree_copy.preorder_begin(); ndi != tree_copy.preorder_end(); ++ndi) {
        std::string s = *ndi;
        uppercase(s);
        *ndi = s;
    }
    if (compare_against_newick_string(tree_original, "original tree with restored labels failed to yield expected newick string", STANDARD_TEST_TREE_NEWICK, false)) {
        return EXIT_FAILURE;
    }
    if (compare_against_newick_string(tree_copy, "cloned tree yielded expected newick string even though labels modified", STANDARD_TEST_TREE_NEWICK, true)) {
        return EXIT_FAILURE;
    }
    return 0;
}
