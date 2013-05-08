#include "platypus_testing.hpp"

using namespace platypus::test;

int main () {


    std::string tree_string = ";;;;"
        + STANDARD_TEST_TREE_NEWICK
        + ";;;;;"
        + STANDARD_TEST_TREE_NEWICK;
    auto trees = platypus::test::get_test_data_tree_vector_from_string<TestDataTree>(tree_string);

    int fail = 0;
    if (trees.size() != 2) {
        fail = 1;
        std::cerr << "Expecting 2 trees but found " << trees.size() << std::endl;
    }
    for (auto & tree : trees) {
        fail += compare_against_standard_test_tree(tree);
    }
    // try {
    //     tree_reader.read_from_string(tree_string, "newick");
    // } catch (const platypus::NewickReaderMalformedStatementError & e) {
    //     return 0;
    // }
    if (fail == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

