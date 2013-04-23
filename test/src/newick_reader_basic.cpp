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

    bool pass = true;
    std::vector<std::string> postorder_visits;
    for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
        postorder_visits.push_back(ndi->get_label());
    }
    if (postorder_visits != STANDARD_TEST_TREE_POSTORDER) {
        pass = false;
        fail_test(__FILE__,
            STANDARD_TEST_TREE_POSTORDER,
            postorder_visits);
    }

    std::vector<std::string> preorder_visits;
    for (auto ndi = tree.preorder_begin(); ndi != tree.preorder_end(); ++ndi) {
        preorder_visits.push_back(ndi->get_label());
    }
    if (preorder_visits != STANDARD_TEST_TREE_PREORDER) {
        pass = false;
        fail_test(__FILE__,
            STANDARD_TEST_TREE_PREORDER,
            preorder_visits);
    }

    if (pass) {
        return 0;
    } else {
        return 1;
    }
    // tree_reader.read_from_string("(a b); ", "newick"); // test case: missing comma
    // tree_reader.read_from_string("(a, b, ,,); ", "newick"); // test case: extra commas (should be treated as blank nodes);
    // tree_reader.read_from_string("(a, (b, c)) ", "newick"); // test case: missing semi-colon;
    // tree_reader.read_from_string("(a:0, b:0, :0, :0):0;", "newick"); // test case: empty nodes with brlens

}

