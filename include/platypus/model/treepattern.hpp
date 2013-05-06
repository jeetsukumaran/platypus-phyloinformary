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

#include <stdexcept>
#include <stack>
#include <set>
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
inline void build_maximally_unbalanced_tree(
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
    auto insert_leaf_node = [&leaf_nodes, &apical_node] () {
        apical_node->add_child(leaf_nodes.front());
        leaf_nodes.pop();
    };
    if (ladderize_right) {
        auto insert_internal_node = [&apical_node, &tree] () {
                typename TreeT::node_type * node = tree.create_internal_node();
                apical_node->add_child(node);
                apical_node = node;
        };
        while (!leaf_nodes.empty()) {
            insert_leaf_node();
            if (leaf_nodes.size() > 1) {
                insert_internal_node();
            } else {
                insert_leaf_node();
            }
        }
    } else {
        while (!leaf_nodes.empty()) {
            if (leaf_nodes.size() > 2) {
                typename TreeT::node_type * node = tree.create_internal_node();
                apical_node->add_child(node);
                insert_leaf_node();
                apical_node = node;
            } else {
                insert_leaf_node();
                insert_leaf_node();
            }
        }
    }
}


/**
 * Generates a balanced or symmetric tree.
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
inline void build_maximally_balanced_tree(
        TreeT & tree,
        LeafIterT leaf_values_begin,
        LeafIterT leaf_values_end) {
    std::stack<typename TreeT::node_type *> node_pool;
    for (auto & leaf_iter = leaf_values_begin; leaf_iter != leaf_values_end; ++leaf_iter) {
        typename TreeT::node_type * node = tree.create_leaf_node(*leaf_iter);
        node_pool.push(node);
    }
    while (node_pool.size() > 2) {
        std::set<typename TreeT::node_type *> nodes_to_add;
        while (node_pool.size() > 1) {
            if (node_pool.size() > 1) {
                typename TreeT::node_type * node = tree.create_internal_node();
                node->add_child(node_pool.top());
                node_pool.pop();
                node->add_child(node_pool.top());
                node_pool.pop();
                nodes_to_add.insert(node);
            } else {
                break;
            }
        }
        for (auto & nd : nodes_to_add) {
            node_pool.push(nd);
        }
    }
    tree.head_node()->add_child(node_pool.top());
    node_pool.pop();
    tree.head_node()->add_child(node_pool.top());
    node_pool.pop();
}


} // platypus

#endif
