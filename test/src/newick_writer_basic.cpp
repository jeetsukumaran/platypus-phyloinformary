#include <sstream>
#include "tests.hpp"

int main () {

    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = get_test_data_tree_newick_reader(trees);
    std::string tree_string = STANDARD_TEST_TREE_NEWICK;
    tree_reader.read_from_string(tree_string, "newick");

    platypus::NewickWriter<TreeType>  newick_writer;
    newick_writer.set_tree_is_rooted_getter([](const TreeType & tree)->bool {return tree.is_rooted();} );
    newick_writer.set_node_label_getter([](const TreeType::value_type & nv)->std::string {return nv.get_label();} );
    newick_writer.set_edge_length_getter([](const TreeType::value_type & nv)->double {return nv.get_edge_length();} );

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
        fail_test(__FILE__,
                tree_string2,
                tree_string3);
        return 1;
    } else {
        return 0;
    }
}

