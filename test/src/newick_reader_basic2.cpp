#include <sstream>
#include <map>
#include <string>
#include "tests.hpp"

template <class TreeT>
void write_newick_no_brlens(const TreeT& tree, std::ostream& out) {
    const std::function<void (typename TreeT::value_type &, std::ostream &)> write_node_f(
            [] (typename TreeT::value_type & nv, std::ostream & out) { out << nv.get_label(); });
    write_newick(tree, out, write_node_f);
}

int main () {
    typedef TestDataTree TreeType;
    // std::string tree_string = "(a,(b,(c,d)));";
    std::map<std::string, std::string> tree_strings({
        {"(a,(b,(c,d)));",   "(a,(b,(c,d)));"}
    });
    int fail = 0;
    for (auto & si : tree_strings) {
        auto & src = si.first;
        auto & expected = stripspaces(si.second);
        TreeType tree;
        auto tree_reader = get_single_tree_newick_reader(tree);
        tree_reader.read_from_string(src);
        std::stringstream o;
        write_newick_no_brlens(tree, o);
        std::string observed = o.str();
        stripspaces(observed);
        if (observed != expected) {
            fail += 1;
            fail_test(__FILE__,
                expected,
                observed,
                "Input tree string: '", src, "'");
        }
    }
    return fail;
}

