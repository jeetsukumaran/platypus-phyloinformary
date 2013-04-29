#include <sstream>
#include <iostream>
#include <vector>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include "tests.hpp"

using namespace platypus::test;

int main() {
    // tree source
    std::string tree_string;
    for (int i = 0; i < 5; ++i) {
        tree_string += STANDARD_TEST_TREE_NEWICK;
    }

    // tree
    typedef platypus::StandardTree<platypus::StandardNodeValue> TreeType;
    std::vector<TreeType> trees;
    auto tree_factory = [&trees] () -> TreeType & { trees.emplace_back(true); return trees.back(); };

    // reading
    auto reader = platypus::NewickReader<TreeType>(tree_factory);
    configure_producer_for_standard_interface(reader);
    reader.read_from_string(tree_string);

    // check reading
    if (trees.size() != 5) {
        return 1;
    }

    // writer
    auto writer = platypus::NewickWriter<TreeType>();
    configure_writer_for_standard_interface(writer);
    std::ostringstream out;
    writer.write(trees.begin(), trees.end(), out);
    auto result = out.str();

    std::string expected =
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n"
        "[&R] ((i:0, (j:0, k:0)e:0)b:0, ((l:0, m:0)g:0, (n:0, (o:0, p:0)h:0)f:0)c:0)a:0;\n";

    return check_equal(expected, result, __FILE__, __LINE__);
}
