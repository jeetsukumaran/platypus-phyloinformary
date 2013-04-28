#include <sstream>
#include <iostream>
#include <vector>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include "tests.hpp"

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
    if (trees.size() != num_trees) {
        fail += fail_test(__FILE__, trees.size(), num_trees, "(number of trees)");
    }

    for (auto & tree : trees) {
        if (tree.num_leaves != 8) {
            fail += fail_test(__FILE__, 8, tree.num_leaves, "(number of leaves)");
        }
        if (tree.num_internal_nodes != 7) {
            fail += fail_test(__FILE__, 7, tree.num_internal_nodes, "(number of internal nodes)");
        }
        if (tree.tree_length != 0) {
            fail += fail_test(__FILE__, 0, tree.tree_length, "(tree length)");
        }
    }
    return fail;

}
