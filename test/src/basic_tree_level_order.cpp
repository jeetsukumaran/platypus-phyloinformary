#include <stdlib.h>
#include <algorithm>
#include <stack>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include "platypus_testing.hpp"

using namespace platypus::test;

int check_visits(
        const std::string & label,
        const std::vector<std::string> & visits,
        const std::vector<std::string> & expected_label_groups) {
    std::cerr << "-- " << label << std::endl;
    int fails = 0;
    std::vector<std::string> expected = expected_label_groups;
    typename std::vector<std::string>::iterator current_expected = expected.begin();
    for (auto current_obs = visits.begin(); current_obs != visits.end(); ++current_obs) {
        if (current_expected->empty()) {
            ++current_expected;
        }
        if (current_expected == STANDARD_TEST_TREE_LEVEL_ORDER.end()) {
            fails += 1;
            std::cerr << "***FAIL*** Exhausted expected, but " << visits.end() - current_obs << " nodes remaining in observed" << std::endl;
            return fails;
        }
        auto found = (*current_expected).find((*current_obs)[0]);
        if (found == std::string::npos) {
            fails += 1;
            std::cerr << "***FAIL*** Observed label '" << *current_obs << "' not found in expected group: '" << *current_expected << "'" << std::endl;
            return fails;
        } else {
            std::cerr << "OK: '" << *current_obs << "' found in '" << *current_expected << "'" << std::endl;
            current_expected->erase(found, 1);
        }
    }
    return fails;
}

int test_level_order_iterator() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::vector<std::string> visits;
    for (auto ndi = tree.level_order_begin(); ndi != tree.level_order_end(); ++ndi) {
        visits.push_back(*ndi);
    }
    return check_visits("level-order, internal + leaves", visits, STANDARD_TEST_TREE_LEVEL_ORDER);
}

int test_level_order_rev_iterator() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::vector<std::string> visits;
    for (auto ndi = tree.level_order_rbegin(); ndi != tree.level_order_rend(); ++ndi) {
        visits.push_back(*ndi);
    }
    auto expected = STANDARD_TEST_TREE_LEVEL_ORDER;
    std::reverse(expected.begin(), expected.end());
    return check_visits("reverse level-order, internal + leaves", visits, expected);
}

int test_level_order_internal_iterator() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::vector<std::string> visits;
    for (auto ndi = tree.level_order_begin(false); ndi != tree.level_order_end(); ++ndi) {
        visits.push_back(*ndi);
    }
    return check_visits("level-order, internal only", visits, STANDARD_TEST_TREE_INTERNAL_LEVEL_ORDER);
}

int test_level_order_rev_internal_iterator() {
    BasicTree tree;
    build_tree(tree, STANDARD_TEST_TREE_STRING);
    std::vector<std::string> visits;
    for (auto ndi = tree.level_order_rbegin(false); ndi != tree.level_order_rend(); ++ndi) {
        visits.push_back(*ndi);
    }
    auto expected = STANDARD_TEST_TREE_INTERNAL_LEVEL_ORDER;
    std::reverse(expected.begin(), expected.end());
    return check_visits("reverse level-order, internal only", visits, expected);
}


int main() {
    int fails = 0;
    fails += test_level_order_iterator();
    fails += test_level_order_rev_iterator();
    fails += test_level_order_internal_iterator();
    fails += test_level_order_rev_internal_iterator();
    if (fails == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
