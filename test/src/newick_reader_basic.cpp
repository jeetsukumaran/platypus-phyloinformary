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
#include <platypus/treereader.hpp>

namespace platypus {

template <typename TreeT>

class NewickTreeReader : public TreeReader<TreeT> {
    public:

        NewickTreeReader(
                const typename TreeReader<TreeT>::tree_factory_fntype & tree_factory,
                const typename TreeReader<TreeT>::tree_is_rooted_setter_fntype & tree_is_rooted_func,
                const typename TreeReader<TreeT>::node_value_label_setter_fntype & node_value_label_func,
                const typename TreeReader<TreeT>::node_value_edge_length_setter_fntype & node_value_edge_length_func)
            : TreeReader<TreeT>(tree_factory,
                    tree_is_rooted_func,
                    node_value_label_func,
                    node_value_edge_length_func) {
        }

        int parse_from_stream(std::istream& src, const std::string& format="newick") override {
            if (format != "newick") {
                throw std::logic_error("NewickReader only supports 'newick' formatted sources");
            }
            return 0;
        }
    protected:

}; // NewickTreeReader

} // namespace platypus

int main () {

    std::string s = STANDARD_TEST_TREE_NEWICK;

    return 1;
}

