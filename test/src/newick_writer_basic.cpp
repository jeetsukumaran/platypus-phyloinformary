#include <sstream>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main () {

    std::string tree_string = STANDARD_TEST_TREE_NEWICK;
    auto trees = platypus::test::get_test_data_tree_vector_from_string<TestDataTree>(tree_string);

    platypus::NewickWriter<TestDataTree>  newick_writer = get_standard_newick_writer<TestDataTree>();

    std::ostringstream o1;
    // newick_writer.write(trees.cbegin(), trees.cend(), o1);
    newick_writer.write(o1, trees.cbegin(), trees.cend());

    std::string tree_string2 = o1.str();
    auto trees2 = platypus::test::get_test_data_tree_vector_from_string<TestDataTree>(tree_string2);
    std::ostringstream o2;
    // newick_writer.write(trees2.cbegin(), trees2.cend(), o2);
    newick_writer.write(o2, trees2.cbegin(), trees2.cend());
    std::string tree_string3 = o2.str();
    int chk = platypus::testing::compare_equal( tree_string2, tree_string3, __FILE__, __LINE__);
    if (chk == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

