#include <sstream>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main () {

    typedef TestDataTree TreeType;
    std::vector<std::string> labels{"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    std::vector<TestData> leaves;
    leaves.reserve(labels.size());
    for (auto & label : labels) {
        leaves.emplace_back(label);
    }
    TreeType tree;
    build_maximally_balanced_tree(tree, leaves.begin(), leaves.end());
    std::ostringstream o;
    auto newick_writer = get_standard_newick_writer<TreeType>(false);
    newick_writer.set_suppress_rooting(true);
    newick_writer.set_suppress_edge_lengths(true);
    newick_writer.set_compact_spaces(true);
    newick_writer.write_tree(tree, o);
    std::string result = o.str();
    std::string expected =  "((((f,e),(h,g)),((b,a),(d,c))),(j,i));";
    int chk = platypus::testing::compare_equal(expected, result, __FILE__, __LINE__);
    if (chk == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

