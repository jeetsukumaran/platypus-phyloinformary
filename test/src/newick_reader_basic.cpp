#include <string>
#include <sstream>
#include <memory>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include <platypus/model/standardinterface.hpp>
#include "platypus_testing.hpp"

static const unsigned int DEFAULT_NUM_TREES = 5;

std::string get_multitree_string(unsigned int num_trees=DEFAULT_NUM_TREES) {
    std::ostringstream o;
    for (unsigned int i = 0; i < num_trees; ++i) {
        o << platypus::test::STANDARD_TEST_TREE_NEWICK << "\n";
    }
    return o.str();
}

int primitive_node_value_type_tree_read() {
    int fails = 0;
    typedef platypus::Tree<char> TreeType;
    std::vector<TreeType> trees;
    auto reader = platypus::NewickReader<TreeType>();
    reader.set_node_label_setter([](char & s, std::string label){s = label[0];});
    reader.read(std::istringstream(get_multitree_string()),
            [&trees]()->TreeType&{trees.emplace_back(); return trees.back();}
            );
    fails += platypus::testing::compare_equal(
            DEFAULT_NUM_TREES,
            trees.size(),
            __FILE__,
            __LINE__,
            "Number of trees in vector");
    std::function<std::string (const char &)> get_char_label = [](const char & nv)->std::string {std::string x(1, nv); return x;};
    for (auto & tree : trees) {
        fails += platypus::test::compare_against_standard_test_tree(tree, get_char_label);
    }
    return fails;
}

int class_node_value_type_tree_read() {
    int fails = 0;
    typedef platypus::test::TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto reader = platypus::NewickReader<TreeType>();
    platypus::bind_standard_interface(reader);
    reader.read(std::istringstream(get_multitree_string()),
            [&trees]()->TreeType&{trees.emplace_back(); return trees.back();}
            );
    fails += platypus::testing::compare_equal(
            DEFAULT_NUM_TREES,
            trees.size(),
            __FILE__,
            __LINE__,
            "Number of trees in vector");
    for (auto & tree : trees) {
        fails += platypus::test::compare_against_standard_test_tree(tree);
    }
    return fails;
}

int tree_ptrs_read() {
    int fails = 0;
    typedef platypus::test::TestDataTree TreeType;
    std::vector<std::unique_ptr<TreeType>> trees;
    auto reader = platypus::NewickReader<TreeType>();
    platypus::bind_standard_interface(reader);
    reader.read(std::istringstream(get_multitree_string()),
            [&trees]()->TreeType&{trees.emplace_back(new TreeType()); return *trees.back();}
            );
    fails += platypus::testing::compare_equal(
            DEFAULT_NUM_TREES,
            trees.size(),
            __FILE__,
            __LINE__,
            "Number of trees in vector");
    for (auto & tree : trees) {
        fails += platypus::test::compare_against_standard_test_tree(*tree);
    }
    return fails;
}

int main () {
    primitive_node_value_type_tree_read();
    class_node_value_type_tree_read();
}

