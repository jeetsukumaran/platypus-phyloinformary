#include "platypus_testing.hpp"

using namespace platypus::test;

int main () {

    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = get_test_data_tree_newick_reader(trees);

    std::string tree_string = "((a, c) (b, d));";
    try {
        tree_reader.read_from_string(tree_string, "newick");
    } catch (const platypus::NewickReaderMalformedStatement & e) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

