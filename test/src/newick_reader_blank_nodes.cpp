#include <sstream>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include <platypus/model/standardinterface.hpp>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main () {

    std::string tree_string = "(,(,(,)));";
    auto trees = platypus::test::get_test_data_tree_vector_from_string<TestDataTree>(tree_string);
    assert(trees.size() == 1);
    auto tree = trees[0];
    int fail = 0;
    int internal_count = 0;
    int leaf_count = 0;
    int node_count = 0;
    for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
        ++node_count;
        if (!ndi->get_label().empty()) {
            fail = 1;
            std::cerr << "Expecting empty node label but found: '" << ndi->get_label() << "'" << std::endl;
        }
        if (ndi.is_leaf()) {
            ++leaf_count;
        } else {
            ++internal_count;
        }
    }
    if (leaf_count != 4) {
        fail = 1;
        std::cerr << "Expecting 4 leaves but found: " << leaf_count << std::endl;
    }
    if (internal_count != 3) {
        fail = 1;
        std::cerr << "Expecting 3 internal nodes but found: " << internal_count << std::endl;
    }
    if (fail > 0) {
        std::ostringstream o;
        write_newick(tree, o);
        std::cerr << o.str() << std::endl;
    }
    if (fail == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

