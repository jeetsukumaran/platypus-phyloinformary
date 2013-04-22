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

#include "tests.hpp"
#include <platypus/datareader.hpp>
#include <platypus/tokenizer.hpp>

namespace platypus {

template <typename TreeT>

class NewickReader : public BaseTreeReader<TreeT> {
    public:

        NewickReader(
                const typename BaseTreeReader<TreeT>::tree_factory_fntype & tree_factory,
                const typename BaseTreeReader<TreeT>::tree_is_rooted_setter_fntype & tree_is_rooted_func,
                const typename BaseTreeReader<TreeT>::node_value_label_setter_fntype & node_value_label_func,
                const typename BaseTreeReader<TreeT>::node_value_edge_length_setter_fntype & node_value_edge_length_func)
            : BaseTreeReader<TreeT>(tree_factory,
                    tree_is_rooted_func,
                    node_value_label_func,
                    node_value_edge_length_func) {
        }

        NewickReader() {
        }

        int parse_from_stream(std::istream& src, const std::string& format="newick") override {
            if (format != "newick") {
                throw std::logic_error("platypus::NewickReader only supports 'newick' formatted sources");
            }
            while (true) {
            }
            return 0;
        }

    protected:

}; // NewickTreeReader

} // namespace platypus

struct NodeData {
    std::string label;
    double      edge_length;
};
int main () {
    typedef platypus::Tree<NodeData> TreeType;
    std::vector<TreeType> trees;
    auto tree_factory = [&trees] () -> TreeType& { trees.emplace_back(); return trees.back(); };
    auto is_rooted_f = [] (TreeType& tree, bool) {}; // no-op
    auto node_label_f = [] (NodeData& nd, const std::string& label) {nd.label = label;};
    auto node_edge_f = [] (NodeData& nd, double len) {nd.edge_length = len;};
    platypus::NewickReader<TreeType> tree_reader;
    tree_reader.set_tree_factory(tree_factory);
    tree_reader.set_tree_is_rooted_setter(is_rooted_f);
    tree_reader.set_node_label_setter(node_label_f);
    tree_reader.set_edge_length_setter(node_edge_f);
    tree_reader.read_from_string("[&R](A:1,(B:1,(D:1,(E:1,F:1):1):1):1):1;", "newick");
}

