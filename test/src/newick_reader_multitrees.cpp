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
        fail += validate_standard_test_tree(tree);
    }

    return fail;
}

