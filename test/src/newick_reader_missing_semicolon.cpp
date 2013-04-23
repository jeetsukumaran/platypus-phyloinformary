#include <platypus/newick.hpp>
#include "tests.hpp"

int main () {

    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = get_test_data_tree_newick_reader(trees);

    std::string tree_string = "(a, (b, c));";
    try {
        tree_reader.read_from_string(tree_string, "newick");
    } catch (const platypus::NewickReaderMalformedStatement & e) {
        return 0;
    }
    return 1;
}

