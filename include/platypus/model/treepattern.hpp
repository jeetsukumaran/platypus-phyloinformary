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

#include <cassert>
#include <stdexcept>
#include <stack>
#include <set>
#include <queue>

namespace platypus {

namespace treepattern { namespace detail {

template <typename TreeT, typename NodeValueT>
typename TreeT::node_type * join_nodes(
        TreeT & tree,
        std::vector<NodeValueT> & node_pool,
        unsigned long start_idx,
        unsigned long stop_idx) {
    assert(stop_idx > start_idx);
    unsigned long span = stop_idx - start_idx;
    if (span == 2) {
        typename TreeT::node_type * node = tree.create_internal_node();
        node->add_child(node_pool[start_idx]);
        node->add_child(node_pool[start_idx+1]);
        return node;
    } else if (span == 3) {
        typename TreeT::node_type * node1 = tree.create_internal_node();
        node1->add_child(node_pool[start_idx]);
        node1->add_child(node_pool[start_idx+1]);
        typename TreeT::node_type * node2 = tree.create_internal_node();
        node2->add_child(node_pool[start_idx+2]);
        node2->add_child(node1);
        return node2;
    } else {
        unsigned long mid = static_cast<unsigned long>(span/2);
        mid = start_idx + mid;
        auto * ch1 = detail::join_nodes(tree, node_pool, start_idx, mid);
        auto * ch2 = detail::join_nodes(tree, node_pool, mid, stop_idx);
        typename TreeT::node_type * node = tree.create_internal_node();
        node->add_child(ch1);
        node->add_child(ch2);
        return node;
    }
}

} } // namespace treepattern::detail

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
void build_maximally_balanced_tree(
        TreeT & tree,
        LeafIterT leaf_values_begin,
        LeafIterT leaf_values_end) {
    std::vector<typename TreeT::node_type *> node_pool;
    for (auto & leaf_iter = leaf_values_begin; leaf_iter != leaf_values_end; ++leaf_iter) {
        typename TreeT::node_type * node = tree.create_leaf_node(*leaf_iter);
        node_pool.push_back(node);
    }
    unsigned long mid = static_cast<unsigned long>(node_pool.size()/2);
    auto * ch1 = treepattern::detail::join_nodes(tree, node_pool, 0, mid);
    auto * ch2 = treepattern::detail::join_nodes(tree, node_pool, mid, node_pool.size());
    tree.head_node()->add_child(ch1);
    tree.head_node()->add_child(ch2);

    // std::stack<typename treet::node_type *> node_pool;
    // for (auto & leaf_iter = leaf_values_begin; leaf_iter != leaf_values_end; ++leaf_iter) {
    //     typename TreeT::node_type * node = tree.create_leaf_node(*leaf_iter);
    //     node_pool.push(node);
    // }
    // while (node_pool.size() > 2) {
    //     std::set<typename TreeT::node_type *> nodes_to_add;
    //     while (node_pool.size() > 1) {
    //         if (node_pool.size() > 1) {
    //             typename TreeT::node_type * node = tree.create_internal_node();
    //             node->add_child(node_pool.top());
    //             node_pool.pop();
    //             node->add_child(node_pool.top());
    //             node_pool.pop();
    //             nodes_to_add.insert(node);
    //             if (node_pool.size() % 2 != 0) {
    //                 break;
    //             }
    //         } else {
    //             nodes_to_add.insert(node_pool.top());
    //             node_pool.pop();
    //         }
    //     }
    //     for (auto & nd : nodes_to_add) {
    //         node_pool.push(nd);
    //     }
    // }
    // tree.head_node()->add_child(node_pool.top());
    // node_pool.pop();
    // tree.head_node()->add_child(node_pool.top());
    // node_pool.pop();

    // unsigned long num_tips_needed = 0;
    // for (auto leaf_iter = leaf_values_begin; leaf_iter != leaf_values_end; ++leaf_iter) {
    //     ++num_tips_needed;
    // }
    // std::set<typename TreeT::node_type *> current_tips;
    // current_tips.insert(tree.head_node());
    // while (current_tips.size() < num_tips_needed) {
    //     std::set<typename TreeT::node_type *> new_tips;
    //     auto tip_iter = current_tips.begin();
    //     while (tip_iter != current_tips.end()) {
    //         typename TreeT::node_type * new_ch1 = tree.create_node();
    //         typename TreeT::node_type * new_ch2 = tree.create_node();
    //         (*tip_iter)->add_child(new_ch1);
    //         (*tip_iter)->add_child(new_ch2);
    //         new_tips.insert(new_ch1);
    //         new_tips.insert(new_ch2);
    //         current_tips.erase(tip_iter++);
    //         if (current_tips.size() + new_tips.size() >= num_tips_needed) {
    //             break;
    //         }
    //     }
    //     current_tips.insert(new_tips.begin(), new_tips.end());
    // }
    // auto current_leaf_value_iter = leaf_values_begin;
    // // for (auto new_leaf_node_iter = current_tips.begin(); new_leaf_node_iter != current_tips.end(); ++new_leaf_node_iter, ++current_leaf_value_iter) {
    // for (auto new_leaf_node_iter = tree.leaf_begin(); new_leaf_node_iter != tree.leaf_end(); ++new_leaf_node_iter, ++current_leaf_value_iter) {
    //     if (current_leaf_value_iter == leaf_values_end) {
    //         throw std::runtime_error("Leaf values exhausted before all tips on trees assigned values");
    //     }
    //     *new_leaf_node_iter = *current_leaf_value_iter;
    // }
}


} // platypus

#endif
