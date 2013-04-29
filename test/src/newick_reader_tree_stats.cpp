#include <sstream>
#include <iostream>
#include <vector>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include "platypus_testing.hpp"

using namespace platypus::test;

class STree : public platypus::StandardTree<platypus::StandardNodeValue> {

    public:
        unsigned long   num_leaves;
        unsigned long   num_internal_nodes;
        double          tree_length;

};

int main() {
    // tree source
    std::string tree_string;
    unsigned int num_trees = 3;
    for (unsigned int i = 0; i < num_trees; ++i) {
        tree_string += STANDARD_TEST_TREE_NEWICK;
    }

    // tree
    typedef STree TreeType;
    std::vector<TreeType> trees;
    auto tree_factory = [&trees] () -> TreeType & { trees.emplace_back(); return trees.back(); };

    // reading
    auto reader = platypus::NewickReader<TreeType>(tree_factory);
    configure_producer_for_standard_interface(reader);
    reader.set_tree_stats_num_leaf_nodes_setter([](TreeType & tree, unsigned long n) {tree.num_leaves = n;});
    reader.set_tree_stats_num_internal_nodes_setter([](TreeType & tree, unsigned long n) {tree.num_internal_nodes = n;});
    reader.set_tree_stats_tree_length_setter([](TreeType & tree, double n) {tree.tree_length = n;});
    reader.read_from_string(tree_string);

    int fail = 0;
    fail += check_equal(num_trees, trees.size(), __FILE__, __LINE__, "(number of trees)");
    for (auto & tree : trees) {
        fail += check_equal(8UL, tree.num_leaves, __FILE__, __LINE__, "(number of leaves)");
        fail += check_equal(7UL, tree.num_internal_nodes, __FILE__, __LINE__, "(number of internal nodes)");
        fail += check_equal(0, tree.tree_length, __FILE__, __LINE__, "(total tree length)");
    }

    if (fail > 0) {
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }

}
