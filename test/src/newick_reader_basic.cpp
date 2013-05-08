#include <string>
#include <sstream>
#include <memory>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include <platypus/model/standardinterface.hpp>
#include "platypus_testing.hpp"

static const unsigned int DEFAULT_NUM_TREES = 5;
static const unsigned long EXPECTED_NTIPS = 8;
static const unsigned long EXPECTED_NINTS = 7;
static const double EXPECTED_LENGTH = 0.0;

std::string get_multitree_string(unsigned int num_trees=DEFAULT_NUM_TREES) {
    std::ostringstream o;
    for (unsigned int i = 0; i < num_trees; ++i) {
        o << platypus::test::STANDARD_TEST_TREE_NEWICK << "\n";
    }
    return o.str();
}

template <class TreeT>
void postprocess_tree(TreeT & tree, unsigned long idx, unsigned long ntips, unsigned long nints, double tree_length) {
    tree.set_index(idx);
    tree.set_ntips(ntips);
    tree.set_nints(nints);
    tree.set_length(tree_length);
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
    reader.set_tree_postprocess_fn(postprocess_tree<TreeType>);
    reader.read(std::istringstream(get_multitree_string()),
            [&trees]()->TreeType&{trees.emplace_back(); return trees.back();}
            );
    fails += platypus::testing::compare_equal(
            DEFAULT_NUM_TREES,
            trees.size(),
            __FILE__,
            __LINE__,
            "Number of trees in vector");
    unsigned long idx = 0;
    for (auto & tree : trees) {
        fails += platypus::test::compare_against_standard_test_tree(tree);
        fails += platypus::testing::compare_equal(
                idx,
                tree.get_index(),
                __FILE__,
                __LINE__,
                "Tree index");
        fails += platypus::testing::compare_equal(
                EXPECTED_NTIPS,
                tree.get_ntips(),
                __FILE__,
                __LINE__,
                "Number of tips reported");
        fails += platypus::testing::compare_equal(
                EXPECTED_NINTS,
                tree.get_nints(),
                __FILE__,
                __LINE__,
                "Number of internal nodes reported");
        fails += platypus::testing::compare_equal(
                EXPECTED_LENGTH,
                tree.get_length(),
                __FILE__,
                __LINE__,
                "Length of tree reported");
        ++idx;
    }
    return fails;
}

int tree_ptrs_read() {
    int fails = 0;
    typedef platypus::test::TestDataTree TreeType;
    std::vector<std::unique_ptr<TreeType>> trees;
    auto reader = platypus::NewickReader<TreeType>();
    platypus::bind_standard_interface(reader);
    reader.set_tree_postprocess_fn(postprocess_tree<TreeType>);
    reader.read(std::istringstream(get_multitree_string()),
            [&trees]()->TreeType&{trees.emplace_back(new TreeType()); return *trees.back();}
            );
    fails += platypus::testing::compare_equal(
            DEFAULT_NUM_TREES,
            trees.size(),
            __FILE__,
            __LINE__,
            "Number of trees in vector");
    unsigned long idx = 0;
    for (auto & tree : trees) {
        fails += platypus::test::compare_against_standard_test_tree(*tree);
        fails += platypus::testing::compare_equal(
                idx,
                tree->get_index(),
                __FILE__,
                __LINE__,
                "Tree index");
        fails += platypus::testing::compare_equal(
                EXPECTED_NTIPS,
                tree->get_ntips(),
                __FILE__,
                __LINE__,
                "Number of tips reported");
        fails += platypus::testing::compare_equal(
                EXPECTED_NINTS,
                tree->get_nints(),
                __FILE__,
                __LINE__,
                "Number of internal nodes reported");
        fails += platypus::testing::compare_equal(
                EXPECTED_LENGTH,
                tree->get_length(),
                __FILE__,
                __LINE__,
                "Length of tree reported");
        ++idx;
    }
    return fails;
}

int main () {
    primitive_node_value_type_tree_read();
    class_node_value_type_tree_read();
    tree_ptrs_read();
}

