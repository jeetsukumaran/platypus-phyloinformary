#include <sstream>
#include <map>
#include <string>
#include "tests.hpp"

// import sys
// import dendropy
// trees = dendropy.TreeList.get_from_path(sys.argv[1], "nexus")
// for tree in trees:
//     newick = tree.as_string("newick", suppress_rooting=True).replace("\n", "")
//     print('{{"{}", "{}"}},'.format(newick, newick))

template <class TreeT>
void write_newick_no_brlens(const TreeT& tree, std::ostream& out) {
    const std::function<void (typename TreeT::value_type &, std::ostream &)> write_node_f(
            [] (typename TreeT::value_type & nv, std::ostream & out) { out << nv.get_label(); });
    write_newick(tree, out, write_node_f);
}

int main () {
    typedef TestDataTree TreeType;
    // std::string tree_string = "(a,(b,(c,d)));";
    std::map<std::string, std::string> tree_strings{
            {"(t01,(t02,(t03,t04)));", "(t01,(t02,(t03,t04)));"},
            {"((t01,t02),(t03,t04));", "((t01,t02),(t03,t04));"},
            {"(t01,(t02,(t03,(t04,(t05,(t06,(t07,t08)))))));", "(t01,(t02,(t03,(t04,(t05,(t06,(t07,t08)))))));"},
            {"(t01,(t02,(t03,(t04,((t05,t06),(t07,t08))))));", "(t01,(t02,(t03,(t04,((t05,t06),(t07,t08))))));"},
            {"(t01,(t02,(t03,((t04,t05),(t06,(t07,t08))))));", "(t01,(t02,(t03,((t04,t05),(t06,(t07,t08))))));"},
            {"(t01,(t02,((t03,t04),(t05,(t06,(t07,t08))))));", "(t01,(t02,((t03,t04),(t05,(t06,(t07,t08))))));"},
            {"(t01,(t02,((t03,t04),((t05,t06),(t07,t08)))));", "(t01,(t02,((t03,t04),((t05,t06),(t07,t08)))));"},
            {"(t01,(t02,((t03,(t04,t05)),(t06,(t07,t08)))));", "(t01,(t02,((t03,(t04,t05)),(t06,(t07,t08)))));"},
            {"(t01,((t02,t03),(t04,(t05,(t06,(t07,t08))))));", "(t01,((t02,t03),(t04,(t05,(t06,(t07,t08))))));"},
            {"(t01,((t02,t03),(t04,((t05,t06),(t07,t08)))));", "(t01,((t02,t03),(t04,((t05,t06),(t07,t08)))));"},
            {"(t01,((t02,t03),((t04,t05),(t06,(t07,t08)))));", "(t01,((t02,t03),((t04,t05),(t06,(t07,t08)))));"},
            {"(t01,((t02,(t03,t04)),(t05,(t06,(t07,t08)))));", "(t01,((t02,(t03,t04)),(t05,(t06,(t07,t08)))));"},
            {"(t01,((t02,(t03,t04)),((t05,t06),(t07,t08))));", "(t01,((t02,(t03,t04)),((t05,t06),(t07,t08))));"},
            {"((t01,t02),(t03,(t04,(t05,(t06,(t07,t08))))));", "((t01,t02),(t03,(t04,(t05,(t06,(t07,t08))))));"},
            {"((t01,t02),(t03,(t04,((t05,t06),(t07,t08)))));", "((t01,t02),(t03,(t04,((t05,t06),(t07,t08)))));"},
            {"((t01,t02),(t03,((t04,t05),(t06,(t07,t08)))));", "((t01,t02),(t03,((t04,t05),(t06,(t07,t08)))));"},
            {"((t01,t02),((t03,t04),(t05,(t06,(t07,t08)))));", "((t01,t02),((t03,t04),(t05,(t06,(t07,t08)))));"},
            {"((t01,t02),((t03,t04),((t05,t06),(t07,t08))));", "((t01,t02),((t03,t04),((t05,t06),(t07,t08))));"},
            {"((t01,t02),((t03,(t04,t05)),(t06,(t07,t08))));", "((t01,t02),((t03,(t04,t05)),(t06,(t07,t08))));"},
            {"((t01,(t02,t03)),(t04,(t05,(t06,(t07,t08)))));", "((t01,(t02,t03)),(t04,(t05,(t06,(t07,t08)))));"},
            {"((t01,(t02,t03)),(t04,((t05,t06),(t07,t08))));", "((t01,(t02,t03)),(t04,((t05,t06),(t07,t08))));"},
            {"((t01,(t02,t03)),((t04,t05),(t06,(t07,t08))));", "((t01,(t02,t03)),((t04,t05),(t06,(t07,t08))));"},
            {"((t01,(t02,(t03,t04))),(t05,(t06,(t07,t08))));", "((t01,(t02,(t03,t04))),(t05,(t06,(t07,t08))));"},
            {"((t01,(t02,(t03,t04))),((t05,t06),(t07,t08)));", "((t01,(t02,(t03,t04))),((t05,t06),(t07,t08)));"},
            {"(((t01,t02),(t03,t04)),((t05,t06),(t07,t08)));", "(((t01,t02),(t03,t04)),((t05,t06),(t07,t08)));"},
            {"(((t01,(t02,t03)),(t04,(t05,t06))),((t07,t08),(t09,((t10,t11),(t12,(t13,t14))))));", "(((t01,(t02,t03)),(t04,(t05,t06))),((t07,t08),(t09,((t10,t11),(t12,(t13,t14))))));"},
            {"(((t01,(t02,t03)),(t04,(t05,t06))),((t07,t08),((t09,t10),(t11,(t12,(t13,t14))))));", "(((t01,(t02,t03)),(t04,(t05,t06))),((t07,t08),((t09,t10),(t11,(t12,(t13,t14))))));"},
            {"(((t01,(t02,t03)),(t04,(t05,t06))),((t07,t08),((t09,t10),((t11,t12),(t13,t14)))));", "(((t01,(t02,t03)),(t04,(t05,t06))),((t07,t08),((t09,t10),((t11,t12),(t13,t14)))));"},
            {"(((t01,(t02,t03)),(t04,(t05,t06))),((t07,t08),((t09,(t10,t11)),(t12,(t13,t14)))));", "(((t01,(t02,t03)),(t04,(t05,t06))),((t07,t08),((t09,(t10,t11)),(t12,(t13,t14)))));"},
            {"(((t01,(t02,t03)),(t04,(t05,t06))),((t07,(t08,t09)),(t10,(t11,(t12,(t13,t14))))));", "(((t01,(t02,t03)),(t04,(t05,t06))),((t07,(t08,t09)),(t10,(t11,(t12,(t13,t14))))));"},
            {"(((t01,(t02,t03)),(t04,(t05,t06))),((t07,(t08,t09)),(t10,((t11,t12),(t13,t14)))));", "(((t01,(t02,t03)),(t04,(t05,t06))),((t07,(t08,t09)),(t10,((t11,t12),(t13,t14)))));"},
            {"(((t01,(t02,t03)),(t04,(t05,t06))),((t07,(t08,t09)),((t10,t11),(t12,(t13,t14)))));", "(((t01,(t02,t03)),(t04,(t05,t06))),((t07,(t08,t09)),((t10,t11),(t12,(t13,t14)))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),(t10,(t11,(t12,(t13,(t14,t15))))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),(t10,(t11,(t12,(t13,(t14,t15))))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),(t10,(t11,((t12,t13),(t14,t15)))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),(t10,(t11,((t12,t13),(t14,t15)))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),(t10,((t11,t12),(t13,(t14,t15)))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),(t10,((t11,t12),(t13,(t14,t15)))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),((t10,t11),(t12,(t13,(t14,t15)))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),((t10,t11),(t12,(t13,(t14,t15)))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),((t10,t11),((t12,t13),(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),((t10,t11),((t12,t13),(t14,t15))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),((t10,(t11,t12)),(t13,(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,t09))),((t10,(t11,t12)),(t13,(t14,t15))))));"},
            {"(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),(t10,(t11,(t12,(t13,(t14,t15))))))));", "(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),(t10,(t11,(t12,(t13,(t14,t15))))))));"},
            {"(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),(t10,(t11,((t12,t13),(t14,t15)))))));", "(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),(t10,(t11,((t12,t13),(t14,t15)))))));"},
            {"(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),(t10,((t11,t12),(t13,(t14,t15)))))));", "(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),(t10,((t11,t12),(t13,(t14,t15)))))));"},
            {"(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),((t10,t11),(t12,(t13,(t14,t15)))))));", "(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),((t10,t11),(t12,(t13,(t14,t15)))))));"},
            {"(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),((t10,t11),((t12,t13),(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),((t10,t11),((t12,t13),(t14,t15))))));"},
            {"(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),((t10,(t11,t12)),(t13,(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,t09)),((t10,(t11,t12)),(t13,(t14,t15))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,(t09,t10)))),(t11,(t12,(t13,(t14,t15)))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,(t09,t10)))),(t11,(t12,(t13,(t14,t15)))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,(t09,t10)))),(t11,((t12,t13),(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,(t09,t10)))),(t11,((t12,t13),(t14,t15))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,(t09,t10)))),((t11,t12),(t13,(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,((t06,(t07,(t08,(t09,t10)))),((t11,t12),(t13,(t14,t15))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,((t07,t08),(t09,t10))),(t11,((t12,t13),(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,((t06,((t07,t08),(t09,t10))),(t11,((t12,t13),(t14,t15))))));"},
            {"(((t01,t02),(t03,t04)),(t05,((t06,((t07,t08),(t09,t10))),((t11,t12),(t13,(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,((t06,((t07,t08),(t09,t10))),((t11,t12),(t13,(t14,t15))))));"},
            {"(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,(t09,t10))),((t11,t12),(t13,(t14,t15))))));", "(((t01,t02),(t03,t04)),(t05,(((t06,t07),(t08,(t09,t10))),((t11,t12),(t13,(t14,t15))))));"},
            {"(a,(b,(c,d)));",  "(a,(b,(c,d)));"}
    };
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

