#include <iostream>
#include <string>
#include <vector>
#include "coalescent.hpp"

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
    for (unsigned int i = 0; i < 1; ++i) {
        auto tree = sim.generate_fixed_pop_size_tree(10, 1000);
        std::cout << "[&R] ";
        write_newick(tree, std::cout, write_node_f);
    }
    return 0;
}
