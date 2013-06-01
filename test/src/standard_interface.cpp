#include <sstream>
#include <iostream>
#include <vector>
#include <platypus/model/tree.hpp>
#include <platypus/parse/newick.hpp>
#include <platypus/serialize/newick.hpp>
#include <platypus/model/standardinterface.hpp>
#include "platypus_testing.hpp"


int main() {
    // tree source
    std::string tree_string;
    for (int i = 0; i < 5; ++i) {
        tree_string += platypus::test::STANDARD_TEST_TREE_NEWICK;
    }

    // tree
    typedef platypus::StandardTree<platypus::StandardNodeValue<>> TreeType;
    std::vector<TreeType> trees;
    auto reader = platypus::NewickReader<TreeType>();
    bind_standard_interface(reader);
    reader.read(std::istringstream(tree_string),
        [&trees] () -> TreeType & { trees.emplace_back(true); return trees.back(); }
            );

    // check reading
    if (trees.size() != 5) {
        return 1;
    }

    // writer
    auto writer = platypus::NewickWriter<TreeType>();
    bind_standard_interface(writer);
    writer.set_edge_length_precision(1);
    std::ostringstream out;
    writer.write(out, trees.begin(), trees.end());
    auto result = out.str();

    std::string expected =
        "[&R] ((i:0.0, (j:0.0, k:0.0)e:0.0)b:0.0, ((l:0.0, m:0.0)g:0.0, (n:0.0, (o:0.0, p:0.0)h:0.0)f:0.0)c:0.0)a:0.0;\n"
        "[&R] ((i:0.0, (j:0.0, k:0.0)e:0.0)b:0.0, ((l:0.0, m:0.0)g:0.0, (n:0.0, (o:0.0, p:0.0)h:0.0)f:0.0)c:0.0)a:0.0;\n"
        "[&R] ((i:0.0, (j:0.0, k:0.0)e:0.0)b:0.0, ((l:0.0, m:0.0)g:0.0, (n:0.0, (o:0.0, p:0.0)h:0.0)f:0.0)c:0.0)a:0.0;\n"
        "[&R] ((i:0.0, (j:0.0, k:0.0)e:0.0)b:0.0, ((l:0.0, m:0.0)g:0.0, (n:0.0, (o:0.0, p:0.0)h:0.0)f:0.0)c:0.0)a:0.0;\n"
        "[&R] ((i:0.0, (j:0.0, k:0.0)e:0.0)b:0.0, ((l:0.0, m:0.0)g:0.0, (n:0.0, (o:0.0, p:0.0)h:0.0)f:0.0)c:0.0)a:0.0;\n";

    int chk = platypus::testing::compare_equal(expected, result, __FILE__, __LINE__);
    if (chk == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
