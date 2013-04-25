#include <sstream>
#include "tests.hpp"

int main () {

    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = get_test_data_tree_newick_reader(trees);
    std::string tree_string = STANDARD_TEST_TREE_NEWICK;
    tree_reader.read_from_string(tree_string, "newick");

    platypus::NewickWriter<TreeType>  newick_writer = get_standard_newick_writer();

    std::ostringstream o1;
    newick_writer.write(trees.cbegin(), trees.cend(), o1);

    std::vector<TreeType> trees2;
    auto tree_reader2 = get_test_data_tree_newick_reader(trees2);
    std::string tree_string2 = o1.str();
    tree_reader2.read_from_string(tree_string2, "newick");
    std::ostringstream o2;
    newick_writer.write(trees2.cbegin(), trees2.cend(), o2);
    std::string tree_string3 = o2.str();
    if (tree_string2 != tree_string2) {
        return fail_test(__FILE__,
                tree_string2,
                tree_string3);
    } else {
        return 0;
    }
}

