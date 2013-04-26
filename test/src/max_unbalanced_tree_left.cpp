#include <sstream>
#include "tests.hpp"

int main () {

    typedef TestDataTree TreeType;
    std::vector<std::string> labels{"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};
    std::vector<TestData> leaves;
    leaves.reserve(labels.size());
    for (auto & label : labels) {
        leaves.emplace_back(label);
    }
    TreeType tree;
    build_maximally_unbalanced_tree(tree, leaves.begin(), leaves.end(), false);
    std::ostringstream o;
    auto newick_writer = get_standard_newick_writer<TreeType>(false);
    newick_writer.set_suppress_rooting(true);
    newick_writer.set_suppress_edge_lengths(true);
    newick_writer.set_compact_spaces(true);
    newick_writer.write_tree(tree, o);
    std::string result = o.str();
    std::string expected =  "(((((((((i,j),h),g),f),e),d),c),b),a);";
    if (result != expected) {
        return fail_test(__FILE__,
            expected,
            result);
    } else {
        return 0;
    }
}
