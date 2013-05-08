#include <string>
#include <sstream>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include "platypus_testing.hpp"

static const unsigned int DEFAULT_NUM_TREES = 5;

std::string get_multitree_string(unsigned int num_trees=DEFAULT_NUM_TREES) {
    std::ostringstream o;
    for (unsigned int i = 0; i < num_trees; ++i) {
        o << platypus::test::STANDARD_TEST_TREE_NEWICK << "\n";
    }
    return o.str();
}
std::function<std::string (const char &)> get_char_label = [](const char & nv)->std::string {std::string x(1, nv); return x;};

int simple_tree_read() {
    int fails = 0;
    typedef platypus::Tree<char> TreeType;
    std::vector<TreeType> trees;
    auto reader = platypus::NewickReader<TreeType>();
    auto ts = get_multitree_string();
    reader.set_node_label_setter([](char & s, std::string label){s = label[0];});
    // std::function<TreeType & ()> tf = [&trees]()->TreeType&{trees.emplace_back(); return trees.back();};
    // std::istringstream src(ts);
    // reader.read(src, tf);
    reader.read(std::istringstream(ts),
            [&trees]()->TreeType&{trees.emplace_back(); return trees.back();}
            );
    fails += platypus::testing::compare_equal(
            DEFAULT_NUM_TREES,
            trees.size(),
            __FILE__,
            __LINE__,
            "Number of trees in vector");
    for (auto & tree : trees) {
        fails += platypus::test::compare_against_standard_test_tree(tree, get_char_label);
    }
}

int main () {
    simple_tree_read();
}

