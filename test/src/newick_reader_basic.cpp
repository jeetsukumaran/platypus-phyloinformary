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
    return validate_standard_test_tree(tree);

    // tree_reader.read_from_string("(a b); ", "newick"); // test case: missing comma
    // tree_reader.read_from_string("(a, b, ,,); ", "newick"); // test case: extra commas (should be treated as blank nodes);
    // tree_reader.read_from_string("(a, (b, c)) ", "newick"); // test case: missing semi-colon;
    // tree_reader.read_from_string("(a:0, b:0, :0, :0):0;", "newick"); // test case: empty nodes with brlens

}

