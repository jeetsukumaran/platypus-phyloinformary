#include <iostream>
#include <string>
#include <vector>
#include "../include/cmdopt.hpp"
#include "../../../platypus/tree.hpp"
#include "../../../platypus/coalescent.hpp"

template <class TreeT, class iter>
void write_newick_node(const TreeT& tree,
        const iter& tree_iter,
        std::ostream& out,
        const std::function<void (typename TreeT::value_type &, std::ostream&)> & write_node_f) {
    if (!tree_iter.is_leaf()) {
        out << "(";
        int ch_count = 0;
        for (auto chi = tree.children_begin(tree_iter);
                chi != tree.children_end(tree_iter);
                ++chi, ++ch_count) {
            if (ch_count > 0) {
                out << ", ";
            }
            write_newick_node(tree, chi, out, write_node_f);
        }
        out << ")";
    }
    write_node_f(*tree_iter, out);
}

template <class TreeT>
void write_newick(const TreeT& tree,
        std::ostream& out,
        std::function<void (typename TreeT::value_type &, std::ostream&)> write_node_f) {
    write_newick_node(tree, tree.begin(), out, write_node_f);
    out << ";" << std::endl;
}

template <class TreeT>
void write_newick(const TreeT& tree, std::ostream& out) {
    const std::function<void (typename TreeT::value_type &, std::ostream &)> write_node_f(
            [] (typename TreeT::value_type & nv, std::ostream & out) { out << nv; });
    write_newick(tree, out, write_node_f);
}

struct NodeData {
    std::string label;
    double      edge_length;
    NodeData & operator=(const NodeData & nd) {
        this->label = nd.label;
        this->edge_length = nd.edge_length;
        return *this;
    }
};

int main(int argc, const char * argv []) {

    // cmdline::parser args_parser;
    // args_parser.add<unsigned long>("num-trees", 'n', "number of trees to simulate", false, 1);
    // args_parser.add<double>("population-size", 'p', "(haploid) population size", false, 1.0);
    // args_parser.parse_check(argc, argv);
    // auto & rest = args_parser.rest();
    // if (rest.size() == 0) {
    //     std::cerr << "Need to specify the number of tips on each tree" << std::endl;
    //     exit(1);
    // }

    unsigned long num_tips = 10;
    unsigned long num_trees = 1;
    double population_size = 1.0;;
    platypus::OptionParser parser(
            "SimCoalescentTree v1.0.0",
            "Simulate basic coalescent trees using the platypus-phyloinformary library.",
            "%prog [options] <NUM-TIPS>");
    // parser.add_option<unsigned long>(&num_tips, "-t", "--num-tips",
    //                            "number of tips in each tree");
    parser.add_option<unsigned long>(&num_trees, "-t", "--num-trees",
                               "number of trees to simulate (default = %default)");
    parser.add_option<double>(&num_trees, "-p", "--pop-size",
                               "haploid population size (default = %default)");
    parser.parse(argc, argv);

    const auto & args = parser.get_args();
    if (args.size() != 1) {
        std::cerr << "Expecting exactly one argument: number of tips" << std::endl;
        exit(1);
    }
    num_tips = std::atol(args[0].c_str());

    typedef platypus::Tree<NodeData> TreeType;
    std::vector<TreeType> trees;
    auto tree_factory = [&trees] () -> TreeType& { trees.emplace_back(); return trees.back(); };
    auto is_rooted_f = [] (TreeType& tree, bool) { return true; };
    auto node_label_f = [] (NodeData& nd, const std::string& label) {  nd.label = label; };
    auto node_edge_f = [] (NodeData& nd, double len) {nd.edge_length = len;};
    const std::function<void (typename TreeType::value_type &, std::ostream &)> write_node_f(
            [] (typename TreeType::value_type & nv, std::ostream & out) {
                if (!nv.label.empty()) {
                    out << nv.label;
                }
                out << ":" << nv.edge_length;
            });
    platypus::coalescent::BasicCoalescentSimulator<TreeType> sim(tree_factory, is_rooted_f, node_label_f, node_edge_f);
    for (unsigned long i = 0; i < num_trees; ++i) {
        auto tree = sim.generate_fixed_pop_size_tree(num_tips, population_size);
        std::cout << "[&R] ";
        write_newick(tree, std::cout, write_node_f);
    }
    return 0;
}
