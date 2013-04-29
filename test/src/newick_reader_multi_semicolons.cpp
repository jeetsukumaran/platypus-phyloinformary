#include "tests.hpp"

using namespace platypus::test;

int main () {

    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = get_test_data_tree_newick_reader(trees);

    std::string tree_string = ";;;;"
        + STANDARD_TEST_TREE_NEWICK
        + ";;;;;"
        + STANDARD_TEST_TREE_NEWICK;
    tree_reader.read_from_string(tree_string, "newick");

    int fail = 0;
    if (trees.size() != 2) {
        fail = 1;
        std::cerr << "Expecting 2 trees but found " << trees.size() << std::endl;
    }
    for (auto & tree : trees) {
        fail += validate_standard_test_tree(tree);
    }
    // try {
    //     tree_reader.read_from_string(tree_string, "newick");
    // } catch (const platypus::NewickReaderMalformedStatement & e) {
    //     return 0;
    // }
    if (fail > 0) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
}

