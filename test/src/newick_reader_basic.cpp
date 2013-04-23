#include <platypus/newick.hpp>
#include "tests.hpp"

int main () {

    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = get_test_data_tree_newick_reader(trees);
    std::string tree_string = STANDARD_TEST_TREE_NEWICK;
    tree_reader.read_from_string(tree_string, "newick");
    assert(trees.size() == 1);
    auto tree = trees[0];

    int has_failed = 0;

    // postorder
    std::vector<std::string> postorder_visits;
    for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
        postorder_visits.push_back(ndi->get_label());
    }
    if (postorder_visits != STANDARD_TEST_TREE_POSTORDER) {
        has_failed = 1;
        fail_test(__FILE__,
            STANDARD_TEST_TREE_POSTORDER,
            postorder_visits);
    }

    // preorder
    std::vector<std::string> preorder_visits;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        preorder_visits.push_back(ndi->get_label());
    }
    if (preorder_visits != STANDARD_TEST_TREE_PREORDER) {
        has_failed = 1;
        fail_test(__FILE__,
            STANDARD_TEST_TREE_PREORDER,
            preorder_visits);
    }

    // leaves
    std::vector<std::string> leaves;
    for (auto ndi = tree.leaf_begin(); ndi != tree.leaf_end(); ++ndi) {
        leaves.push_back(ndi->get_label());
    }
    if (leaves != STANDARD_TEST_TREE_LEAVES) {
        has_failed = 1;
        fail_test(__FILE__,
            STANDARD_TEST_TREE_LEAVES,
            leaves);
    }

    // children
    std::map<std::string, std::vector<std::string>> children;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        for (auto chi = tree.children_begin(ndi); chi != tree.children_end(); ++chi) {
            children[ndi->get_label()].push_back(chi->get_label());
        }
        auto expected_iter = STANDARD_TEST_TREE_CHILDREN.find(ndi->get_label());
        assert(expected_iter != STANDARD_TEST_TREE_CHILDREN.end());
        auto expected_children = expected_iter->second;
        if (children[ndi->get_label()] != expected_children) {
            has_failed = 1;
            fail_test(__FILE__,
                expected_children,
                children[ndi->get_label()],
                "Parent node: ", *ndi);
        }
    }

    // siblings
    std::map<std::string, std::vector<std::string>> siblings;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        if (ndi == tree.begin()) {
            // skip root
            continue;
        }
        for (auto sib = tree.next_sibling_begin(ndi); sib != tree.next_sibling_end(); ++sib) {
            siblings[ndi->get_label()].push_back(sib->get_label());
        }
        auto expected_iter = STANDARD_TEST_TREE_SIBLINGS.find(ndi->get_label());
        assert(expected_iter != STANDARD_TEST_TREE_SIBLINGS.end());
        auto expected_siblings = expected_iter->second;
        if (siblings[ndi->get_label()] != expected_siblings) {
            has_failed = 1;
            fail_test(__FILE__,
                expected_siblings,
                siblings[ndi->get_label()],
                "Start node: ", *ndi);
        }
    }

    return has_failed;
    // tree_reader.read_from_string("(a b); ", "newick"); // test case: missing comma
    // tree_reader.read_from_string("(a, b, ,,); ", "newick"); // test case: extra commas (should be treated as blank nodes);
    // tree_reader.read_from_string("(a, (b, c)) ", "newick"); // test case: missing semi-colon;
    // tree_reader.read_from_string("(a:0, b:0, :0, :0):0;", "newick"); // test case: empty nodes with brlens

}

