#include <sstream>
#include <map>
#include <platypus/parse/newick.hpp>
#include <platypus/model/tree.hpp>
#include <platypus/model/standardinterface.hpp>
#include "platypus_testing.hpp"

using namespace platypus::test;

int main () {
    std::map<char, std::string> exp_edges_as_str = {
        {'a', "1"},
        {'b', "1e4"},
        {'c', "1e-4"},
        {'d', "1.14e4"},
        {'c', "-1.2e-4"},
        {'e', "2.0"},
        {'f', "4"},
        {'g', "7.1"},
        {'h', "0.1393"},
        {'i', "0.2394"},
        {'j', "3.1313e4"},
        {'k', "0.4496"},
        {'l', "-0.5597"},
        {'m', "-10.6698e3"},
        {'n', "0.7799"},
        {'o', "0.88910"},
        {'p', "0.99911"}
    };
    // std::map<char, double> exp_edges;
    // for (auto & eiter : exp_edges_as_str) {
    //     exp_edges[eiter.first] = std::atof(eiter.second);
    // }
    std::ostringstream o;
    for (auto c : STANDARD_TEST_TREE_NEWICK) {
        if (exp_edges_as_str.find(c) != exp_edges_as_str.end()) {
            o << c << ":" << exp_edges_as_str[c];
        } else {
            o << c;
        }
    }
    auto tree_string = o.str();
    typedef TestDataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_reader = platypus::NewickReader<TreeType>();
    platypus::bind_standard_interface(tree_reader);
    tree_reader.read(std::istringstream(tree_string),
            [&trees]()->TreeType&{trees.emplace_back(); return trees.back();}
            );
    assert(trees.size() == 1);
    auto tree = trees[0];

    int fail = 0;
    for (auto ndi = tree.postorder_begin(); ndi != tree.postorder_end(); ++ndi) {
        char label = ndi->get_label()[0];
        double edge_length = ndi->get_edge_length();
        double exp_edge_length = std::atof(exp_edges_as_str[label].c_str());
        // std::cout << label << ": " << edge_length << " == " << exp_edge_length << std::endl;
        if ( std::fabs(exp_edge_length - edge_length) > 1e-6  ) {
            fail = 1;
            std::cerr << "Node '" << label << "': expecting " << exp_edge_length;
            std::cerr << ", but found " << edge_length << std::endl;
        }
    }
    if (fail == 0) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}

