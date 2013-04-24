#include <vector>
#include "tests.hpp"

int main () {

    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = get_test_data_tree_newick_reader(trees);
    std::string tree_string = "(a,(b,(c,d)));";
    tree_reader.read_from_string(tree_string, "newick");
    assert(trees.size() == 1);
}

