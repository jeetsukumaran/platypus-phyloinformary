#include <sstream>
#include <iostream>
#include <vector>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include "platypus_testing.hpp"

int NUM_TREES = 5;
typedef platypus::StandardTree<platypus::StandardNodeValue> TreeType;

std::string get_tree_string() {
    std::string tree_string;
    for (int i = 0; i < NUM_TREES; ++i) {
        tree_string += platypus::test::STANDARD_TEST_TREE_NEWICK;
    }
    return tree_string;
}

int simple_get() {
    auto reader = platypus::NewickReader<TreeType>();
    bind_standard_interface(reader);
    std::istringstream src(get_tree_string());
    auto trees1 = reader.get_from_stream(src);
    return platypus::testing::compare_equal(
                NUM_TREES,
                trees1.size(),
                __FILE__,
                __LINE__,
                "Incorrect number of trees read from test data");
}

int complex_get() {
    int fails = 0;
    std::vector<TreeType> trees0;
    auto tree_factory = [&trees0] () -> TreeType & { trees0.emplace_back(); return trees0.back(); };
    auto reader = platypus::NewickReader<TreeType>();
    bind_standard_interface(reader);
    std::istringstream src(get_tree_string());
    auto trees1 = reader.get_from_stream(src);
    fails += platypus::testing::compare_equal(
                NUM_TREES,
                trees1.size(),
                __FILE__,
                __LINE__,
                "Incorrect number of trees read from test data");
    fails += platypus::testing::compare_equal(
                0,
                trees0.size(),
                __FILE__,
                __LINE__,
                "Original vector populated");
}

int pointer_get() {
    int fails = 0;
    auto reader = platypus::NewickReader<TreeType>();
    bind_standard_interface(reader);
    std::istringstream src(get_tree_string());
    auto trees1 = reader.get_ptr_vector_from_stream(src);
    fails += platypus::testing::compare_equal(
                NUM_TREES,
                trees1.size(),
                __FILE__,
                __LINE__,
                "Incorrect number of trees read from test data");
    platypus::NewickWriter<TreeType> newick_writer;
    bind_standard_interface(newick_writer);
    std::ostringstream o;
    newick_writer.write(o, trees1.begin(), trees1.end());
    return fails;
}


int main() {
    simple_get();
    complex_get();
    pointer_get();
    // // tree source

    // // tree

    // // default tree vector
    // std::vector<TreeT> trees0;
    // auto tree_factory = [&trees0] () -> TreeT & { trees0.emplace_back(); return trees0.back(); };

    // // reading

}
