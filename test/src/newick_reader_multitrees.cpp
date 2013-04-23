#include <platypus/newick.hpp>
#include "tests.hpp"

int main () {

    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = get_test_data_tree_newick_reader(trees);

    unsigned int num_trees = 20;
    std::string tree_string;
    for (unsigned int n = 0; n < num_trees; ++n) {
        tree_string += STANDARD_TEST_TREE_NEWICK;
    }

    tree_reader.read_from_string(tree_string, "newick");

    if (trees.size() != num_trees) {
        return fail_test(__FILE__,
                num_trees,
                trees.size(),
                "Incorrect number of trees read from test data");
    }

    bool fail = 0;
    for (auto & tree : trees) {
        std::vector<std::string> postorder_visits;
        for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
            postorder_visits.push_back(ndi->get_label());
        }
        if (postorder_visits != STANDARD_TEST_TREE_POSTORDER) {
            fail = 1;
            fail_test(__FILE__,
                STANDARD_TEST_TREE_POSTORDER,
                postorder_visits);
        }
    }

    return fail;
    // tree_reader.read_from_string("(a b); ", "newick"); // test case: missing comma
    // tree_reader.read_from_string("(a, b, ,,); ", "newick"); // test case: extra commas (should be treated as blank nodes);
    // tree_reader.read_from_string("(a, (b, c)) ", "newick"); // test case: missing semi-colon;
    // tree_reader.read_from_string("(a:0, b:0, :0, :0):0;", "newick"); // test case: empty nodes with brlens

}

