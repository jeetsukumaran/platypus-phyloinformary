/**
 * @package     platypus-phyloinformary
 * @brief       Newick-format tree reader.
 * @author      Jeet Sukumaran
 * @copyright   Copyright (C) 2013 Jeet Sukumaran.
 *              This file is part of "platypus-phyloinformary".
 *              "platypus-phyloinformary" is free software: you can
 *              redistribute it and/or modify it under the terms of the GNU
 *              General Public License as published by the Free Software
 *              Foundation, either version 3 of the License, or (at your
 *              option) any later version.  "platypus-phyloinformary" is
 *              distributed in the hope that it will be useful, but WITHOUT ANY
 *              WARRANTY; without even the implied warranty of MERCHANTABILITY
 *              or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *              Public License for more details.  You should have received a
 *              copy of the GNU General Public License along with
 *              "platypus-phyloinformary".  If not, see
 *              <http://www.gnu.org/licenses/>.
 *
 */

#include <platypus/newick.hpp>
#include "tests.hpp"

int main () {

    std::string tree_string =
        "[&R] ((((T0:0.083, T5:0.083):0.12, T1:0.2):0.64, T4:0.84):0.32, (T2:0.042, T3:0.042):1.1):0;"
        "[&R] ((T1:1, T0:1):2, ((T4:0.14, T2:0.14):0.53, (T5:0.1, T3:0.1):0.57):2.4):0;"
        "[&R] ((T3:0.75, (T2:0.32, ((T0:0.067, T5:0.067):0.05, T1:0.12):0.21):0.43):0.044, T4:0.8):0;"
        "[&R] (((T1:0.3, T5:0.3):0.079, (T3:0.25, (T0:0.076, T2:0.076):0.17):0.13):0.81, T4:1.2):0;"
        "[&R] ((T2:0.075, T5:0.075):0.45, ((T3:0.31, (T4:0.078, T0:0.078):0.23):0.058, T1:0.36):0.16):0;"
        ;

    typedef DataTree TreeType;
    std::vector<TreeType> trees;
    auto tree_factory = [&trees] () -> TreeType& { trees.emplace_back(); return trees.back(); };
    auto is_rooted_f = [] (TreeType& tree, bool) {}; // no-op
    auto node_label_f = [] (TestData& value, const std::string& label) {value.set_label(label);};
    auto node_edge_f = [] (TestData& value, double len) {value.set_edge_length(len);};
    platypus::NewickReader<TreeType> tree_reader;
    tree_reader.set_tree_factory(tree_factory);
    tree_reader.set_tree_is_rooted_setter(is_rooted_f);
    tree_reader.set_node_label_setter(node_label_f);
    tree_reader.set_edge_length_setter(node_edge_f);

    tree_reader.read_from_string(tree_string, "newick");
    // tree_reader.read_from_string("(a b); ", "newick"); // test case: missing comma
    // tree_reader.read_from_string("(a, b, ,,); ", "newick"); // test case: extra commas (should be treated as blank nodes);
    // tree_reader.read_from_string("(a, (b, c)) ", "newick"); // test case: missing semi-colon;
    // tree_reader.read_from_string("(a:0, b:0, :0, :0):0;", "newick"); // test case: empty nodes with brlens

    for (auto & tree : trees) {
        write_newick(tree, std::cout);
    }
}

