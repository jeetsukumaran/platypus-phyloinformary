/**
 * @package     platypus-phyloinformary
 * @brief       Coalescent statistics and computing support.
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

#ifndef PLATYPUS_COALSCENT_HPP
#define PLATYPUS_COALSCENT_HPP

#include <map>
#include "numeric/rng.hpp"
#include "numeric/functions.hpp"
#include "tree.hpp"

namespace platypus {
namespace coalescent {

/**
 * Return a random waiting time for coalescence of any two gene lineages from a
 * sample of `num_lineages` evolving in a population with a haploid size of
 * `haploid_pop_size`.  This is a random draw from the "Kingman" distribution
 * (continuous time version), representing the waiting time to the next
 * coalescent event, or the ime to go from $n$ genes/lineages to $n-1$
 * genes/lineages in a population with haploid size of `haploid_pop_size`,
 * where $n$ = `num_lineages`.
 *
 * Given the number of gene lineages in a sample, `num_lineages`, and a
 * population size, `haploid_pop_size`, this function returns a random
 * number from an exponential distribution with rate
 * $\choose(`haploid_pop_size`, 2)$.  `haploid_pop_size` is the effective
 * *haploid* population size; i.e., the total number of gene lineages in the
 * population: $2 * N$ in a diploid population of $N$ individuals, or $N$ in a
 * haploid population of $N$ individuals. If `haploid_pop_size` is 1 or 0 or
 * None, then time is in haploid population units; i.e. where 1 unit of time
 * equals $2N$ generations for a diploid population of size $N$, or $N$
 * generations for a haploid population of size $N$. Otherwise time is in
 * generations.
 *
 * @note
 * The coalescence time, or the waiting time for the coalescence, of two
 * gene lineages evolving in a population with haploid size $N$ is an
 * exponentially-distributed random variable with rate of $N$ an
 * expectation of $\frac{1}{N}$).
 * The waiting time for coalescence of *any* two gene lineages in a sample of
 * $n$ gene lineages evolving in a population with haploid size $N$ is an
 * exponentially-distributed random variable with rate of $\choose{N, 2}$ and
 * an expectation of $\frac{1}{\choose{N, 2}}$.
 *
 * @param rng
 *   A RandomNumberGenerator or RandomNumberGeneratorTemplate implementation
 *   object.
 * @param num_lineages
 *   The number of lineages currently evolving in the population.
 * @param haploid_pop_size
 *   The haploid population size. If this is 1, then the returned time value
 *   is in populations units (i.e, $1N$ if a haploid population and $2N$ if a
 *   diploid population). Otherwise, the returned time value is in generation
 *   units.
 * @param num_to_coalesce
 *   The number of lineages that coalesce at the event.
 *
 * @return
 *   The random time (in population units or generations, depending on the
 *   value of `haploid_pop_size` till the next coalescent event.
 */
template <class RngT=platypus::numeric::RandomNumberGenerator>
double random_time_to_coalescence(
        RngT & rng,
        unsigned long num_lineages,
        double haploid_pop_size=1.0,
        unsigned long num_to_coalesce=2) {
    auto rate = platypus::numeric::binomial_coefficient(num_lineages, num_to_coalesce);
    auto tmrca = rng.exponential(rate);
    return tmrca * haploid_pop_size;
}


/**
 * Returns the expectation of time to coalescence of `num_lineages` gene
 * lineages evolving in a population with haploid size `haploid_pop_size`.
 *
 * @param num_lineages
 *   The number of lineages currently evolving in the population.
 * @param haploid_pop_size
 *   The haploid population size. If this is 1, then the returned time value
 *   is in populations units (i.e, $1N$ if a haploid population and $2N$ if a
 *   diploid population). Otherwise, the returned time value is in generation
 *   units.
 *
 * @return
 *   The expected or mean time to coalescence of `num_lineages` gene lineages
 *   evolving in a population with haploid size `haploid_pop_size`.
 */
double expected_time_to_coalescence(
        unsigned long num_lineages,
        double haploid_pop_size=1.0,
        unsigned long num_to_coalesce=2) {
    auto tmrca = 1.0/platypus::numeric::binomial_coefficient(num_lineages, num_to_coalesce);
    return tmrca * haploid_pop_size;
}

/**
 * Simulates a trees under a pure, neutral coalescent regime.
 *
 * @tparam TreeT
 *   Tree type.
 * @tparam RngT
 *   Random number generator type.
 */
template <typename TreeT, typename RngT=platypus::numeric::RandomNumberGenerator>
class BasicCoalescentSimulator : public platypus::TreeBuilder<TreeT> {

    public:

        BasicCoalescentSimulator(RngT & rng,
                const typename TreeBuilder<TreeT>::TreeFactoryType & tree_factory,
                const typename TreeBuilder<TreeT>::TreeIsRootedFuncType & tree_is_rooted_func,
                const typename TreeBuilder<TreeT>::SetNodeValueLabelFuncType & node_value_label_func,
                const typename TreeBuilder<TreeT>::SetNodeValueEdgeLengthFuncType & node_value_edge_length_func)
            : TreeBuilder<TreeT>(
                    tree_factory,
                    tree_is_rooted_func,
                    node_value_label_func,
                    node_value_edge_length_func)
            , rng_ptr_(&rng)
            , allocated_rng_(false) {
        }

        BasicCoalescentSimulator(
                const typename TreeBuilder<TreeT>::TreeFactoryType & tree_factory,
                const typename TreeBuilder<TreeT>::TreeIsRootedFuncType & tree_is_rooted_func,
                const typename TreeBuilder<TreeT>::SetNodeValueLabelFuncType & node_value_label_func,
                const typename TreeBuilder<TreeT>::SetNodeValueEdgeLengthFuncType & node_value_edge_length_func)
            : TreeBuilder<TreeT>(
                    tree_factory,
                    tree_is_rooted_func,
                    node_value_label_func,
                    node_value_edge_length_func)
            , allocated_rng_(true) {
            this->rng_ptr_ = new RngT();
        }

        ~BasicCoalescentSimulator() {
            if (this->allocated_rng_ && this->rng_ptr_) {
                delete this->rng_ptr_;
                this->allocated_rng_ = false;
                this->rng_ptr_ = nullptr;
            }
        }

        /**
         * Generates a coalescent tree under a fixed population size.
         *
         * @param num_leaves
         *   The number of tips on the coalescent tree (i.e., number of genes
         *   to coalesce.
         * @param leaf_values_begin
         *   Iterator to beginning of sequence of leaf values that will
         *   become attached to leaf nodes.
         * @param leaf_values_begin
         *   Iterator to one past the end of sequence of leaf values that will
         *   become attached to leaf nodes.
         * @param configure_node_value
         *   A function that takes a reference to a TreeT::value_type object
         *   represent a new coalescent event node, an integral value
         *   representing the 1-based index of the coalescent event (i.e., 0
         *   for the first event, 1 for the next, etc.) and a real value
         *   representing the time of the coalescent event, and configures/sets
         *   the state of the object accordingly (e.g., by setting its age or
         *   node depth value).
         * @param haploid_pop_size
         *   The hapoloid population size. If 1.0 then time is in population
         *   units (2N for a diploid population and 1N in a haploid
         *   population).
         * @param expected_tmrca
         *   If `true`, then instead of a exponential randomvariate for the
         *   waiting time to coalescent events, the mean time will be used.
         * @return
         *   A reference to the tree simulated.
         */
        template <typename iter>
        TreeT & generate_fixed_pop_size_tree(
                iter leaf_values_begin,
                iter leaf_values_end,
                double haploid_pop_size,
                bool use_expected_tmrca=false) {
            auto & tree = this->tree_factory_();
            this->set_tree_is_rooted_(tree, true);
            double elapsed_time = 0.0;
            std::map<typename TreeT::node_type *, double> nodes;
            for (auto & leaf_iter = leaf_values_begin; leaf_iter != leaf_values_end; ++leaf_iter) {
                typename TreeT::node_type * new_node = tree.create_leaf_node(*leaf_iter);
                nodes[new_node] = 0.0;
            }
            double time_expended = 0.0;
            while (nodes.size() > 1) {
                this->simulate_basic_coalescent_event(
                        tree,
                        nodes,
                        time_expended,
                        haploid_pop_size,
                        0.0,
                        use_expected_tmrca);
            }
            return tree;
        }

        TreeT & generate_fixed_pop_size_tree(unsigned long num_leaves,
                double haploid_pop_size,
                bool use_expected_tmrca=false) {
            std::vector<typename TreeT::value_type> leaves;
            for (unsigned long i = 0; i < num_leaves; ++i) {
                auto label = "T" + std::to_string(i);
                leaves.emplace_back();
                auto & leaf = leaves.back();
                this->set_node_value_label_(leaf, label);
            }
            return this->generate_fixed_pop_size_tree(
                    leaves.begin(),
                    leaves.end(),
                    haploid_pop_size,
                    use_expected_tmrca
                    );
        }

        /**
         * Simulates a single coalescence event in a sample of TreeT::node_type
         * pointers to nodes representing gene lineages evolving in a
         * population with haploid size of `haploid_pop_size`.
         *
         * This function will generate a waiting time to a coalescence event,
         * or coalescence time, $t$.
         * If `use_expected_tmrca` is ``false``, then $t$ will be a random
         * value drawn from an exponential distribution with rate $\choose{n,
         * 2} * N$, where $n$ is the number of gene lineages evolving the
         * population (given by `nodes.size()`) and $N$ is the haploid
         * population size, `haploid_pop_size`.
         * If `use_expected_tmrca` is ``true``, then $t$ will be set to the
         * expected time to coalescence, $\frac{1}{\choose(n, 2) * N}.
         *
         * If `time_available` $> 0$ and $t$ is less than or equal to
         * `time_available` then a coalescence event occurs.
         * In this case, the edge lengths of all nodes in the
         * pool (included the coalesced nodes) will be incremented by $t$, a
         * new node representing the ancestor of two coalesced nodes will be
         * created and inserted in the pool, and the two nodes will be removed
         * from the pool.
         * The variable `time_expended` will be set to the waiting time drawn,
         * $t$, and a pointer to the new ancestral node created will be
         * returned.
         *
         * If $t$ is greater than `time_available` or `time_available` is
         * $<0$, the no coalescence occurs. In this case, the pool remains
         * unchanged. The variable `time_expended` will be set equal to
         * `time_available`, and ``nullptr`` will be returned.
         *
         * @param nodes
         *   Pool of pointers to nodes representing gene lineages still
         *   evolving in the population (map keys) and their corresponding edge
         *   lengths (values)
         * @param haploid_pop_size
         *   Population haploid size, i.e., total number of gene lineages in
         *   the population.
         * @param time_available
         * @param use_expected_tmrca
         *
         * @return
         */
        typename TreeT::node_type * simulate_basic_coalescent_event(
                TreeT & tree,
                std::map<typename TreeT::node_type *, double> & nodes,
                double & time_expended,
                unsigned long haploid_pop_size,
                double time_available=0.0,
                bool use_expected_tmrca=false) {
            if (nodes.size() < 2) {
                time_expended = 0.0;
                return nullptr;
            }
            if (time_available < 0.0) {
                time_available = 0.0;
            }
            if (haploid_pop_size < 0.0) {
                throw std::logic_error("Population size cannot be less than 0");
            }
            double tmrca = 0.0;
            if (use_expected_tmrca) {
                tmrca = expected_time_to_coalescence(nodes.size(), haploid_pop_size, 2);
            } else {
                assert(this->rng_ptr_);
                tmrca = random_time_to_coalescence(*(this->rng_ptr_), nodes.size(), haploid_pop_size, 2);
            }
            tmrca = tmrca * haploid_pop_size;
            if (time_available == 0.0 || tmrca <= time_available) {
                for (auto & nde : nodes) {
                    nde.second += tmrca;
                }
                typename TreeT::node_type * anc = nullptr;
                if (nodes.size() > 2) {
                    anc = tree.create_internal_node();
                    typename TreeT::node_type * ch = nullptr;
                    for (unsigned int i = 0; i < 2; ++i) {
                        auto idx = this->rng_ptr_->uniform_pos_int(nodes.size()-1);
                        auto nd_iter = nodes.begin();
                        std::advance(nd_iter, idx);
                        assert(nd_iter != nodes.end());
                        ch = nd_iter->first;
                        this->set_node_value_edge_length_(ch->data(), nodes[ch]);
                        anc->add_child(ch);
                        nodes.erase(nd_iter);
                    }
                } else {
                    anc = tree.head_node();
                    for (auto & chi : nodes) {
                        anc->add_child(chi.first);
                        this->set_node_value_edge_length_(chi.first->data(), nodes[chi.first]);
                    }
                    nodes.clear();
                }
                nodes[anc] = 0.0;
                time_expended = tmrca;
                return anc;
            } else {
                time_expended = time_available;
            }
            return nullptr;
        }

    private:
        RngT *      rng_ptr_;
        bool        allocated_rng_;

}; // BasicCoalescentSimulator


} // namespace platypus
} // namespace coalescent


#endif
