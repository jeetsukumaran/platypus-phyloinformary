/**
 * @package     platypus-phyloinformary
 * @brief       Simulation of some classical tree shapes.
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
 */

#ifndef PLATYPUS_SIMULATE_ARCHETYPALTREE_HPP
#define PLATYPUS_SIMULATE_ARCHETYPALTREE_HPP

#include <queue>

namespace platypus {

/**
 * Generates a fully-pectinate, comb, or ladderized tree.
 *
 * @tparam TreeT
 * @tparam LeafIterT
 * @param tree
 * @param leaf_values_begin
 *   Iterator to beginning of sequence of leaf values that will
 *   become attached to leaf nodes.
 * @param leaf_values_begin
 *   Iterator to one past the end of sequence of leaf values that will
 *   become attached to leaf nodes.
 */
template <typename TreeT, typename LeafIterT>
void build_maximally_unbalanced_tree(
        TreeT & tree,
        LeafIterT leaf_values_begin,
        LeafIterT leaf_values_end,
        bool ladderize_right=true) {
    std::queue<typename TreeT::node_type *> leaf_nodes;
    for (auto & leaf_iter = leaf_values_begin; leaf_iter != leaf_values_end; ++leaf_iter) {
        typename TreeT::node_type * node = tree.create_leaf_node(*leaf_iter);
        leaf_nodes.push(node);
    }
    auto apical_node = tree.head_node();
    while (!leaf_nodes.empty()) {
        auto & leaf = leaf_nodes.front();
        apical_node->add_child(leaf);
        leaf_nodes.pop();
        if (leaf_nodes.size() > 1) {
            typename TreeT::node_type * node = tree.create_internal_node();
            apical_node->add_child(node);
            apical_node = node;
        } else {
            auto & leaf = leaf_nodes.front();
            apical_node->add_child(leaf);
            leaf_nodes.pop();
        }
    }
}


} // platypus

#endif
