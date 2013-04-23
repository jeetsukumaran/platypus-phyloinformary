/**
 * @package     platypus-phyloinformary
 * @brief       Random-number generation.
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

#include <ctime>
#include <random>

#ifndef PLATYPUS_NUMERIC_RNG_HPP
#define PLATYPUS_NUMERIC_RNG_HPP

namespace platypus {
namespace numeric {

////////////////////////////////////////////////////////////////////////////////
// RandomNumberGeneratorTemplate

/**
 * Generation of random numbers under different distributions, templated over
 * different random engines.
 *
 * Possible implementations include:
 *
 *      RandomNumberGeneratorTemplate<std::minstd_rand0>      rng_minstd_rand0;
 *      RandomNumberGeneratorTemplate<std::minstd_rand>       rng_minstd_rand;
 *      RandomNumberGeneratorTemplate<std::mt19937>           rng_mt19937;
 *      RandomNumberGeneratorTemplate<std::mt19937_64>        rng_mt19937_64;
 *      RandomNumberGeneratorTemplate<std::ranlux24_base>     rng_ranlux24_base;
 *      RandomNumberGeneratorTemplate<std::ranlux48_base>     rng_ranlux48_base;
 *      RandomNumberGeneratorTemplate<std::ranlux24>          rng_ranlux24;
 *      RandomNumberGeneratorTemplate<std::ranlux48>          rng_ranlux48;
 *      RandomNumberGeneratorTemplate<std::knuth_b>           rng_knuth_b;
 *
 * @tparam EngineT
 *   One of:
 *       std::minstd_rand0
 *       std::minstd_rand
 *       std::mt19937
 *       std::mt19937_64
 *       std::ranlux24_base
 *       std::ranlux48_base
 *       std::ranlux24
 *       std::ranlux48
 *       std::knuth_b
 */
template <typename EngineT=std::mt19937_64>
class RandomNumberGeneratorTemplate {

    public:
        typedef typename EngineT::result_type RandomSeedType;

    public:

        RandomNumberGeneratorTemplate() {
            this->set_seed_from_time();
        }
        RandomNumberGeneratorTemplate(RandomSeedType rng_seed) {
            this->set_seed_(rng_seed);
        }
        ~RandomNumberGeneratorTemplate() {};

        RandomSeedType get_seed() const {
            return this->engine_;
        }

        void set_seed(typename EngineT::result_type seed) {
            this->seed_ = seed;
            this->engine_.seed(seed);
        }

        void set_seed_from_time() {
            this->set_seed(static_cast<typename EngineT::result_type>(std::time(NULL)));
        }

        void set_seed_from_from_device() {
            std::random_device rd;
            this->set_seed(rd());
        }

        // returns integer value uniformly distributed in [a, b]
        inline double uniform_int(long a, long b) {
            return this->uniform_int_(this->engine_,
                    typename decltype(this->uniform_int_rng_)::param_type (a, b));
        }

        // returns integer value uniformly distributed in [0, b], where a >= 0.
        inline double uniform_pos_int(unsigned long b) {
            return this->uniform_pos_int_rng_(this->engine_,
                    typename decltype(this->uniform_pos_int_rng_)::param_type (0, b));
        }

        // returns integer value uniformly distributed in [a, b], where a >= 0
        inline double uniform_pos_int(unsigned long a, unsigned long b) {
            return this->uniform_pos_int_rng_(this->engine_,
                    typename decltype(this->uniform_pos_int_rng_)::param_type (a, b));
        }

        // returns real value uniformly distributed in [0, 1]
        inline double uniform_real() {
            return this->uniform_real_rng_(this->engine_,
                    typename decltype(this->uniform_real_rng_)::param_type (0.0, 1.0));
        }

        // // returns real value uniformly distributed in [a, b]
        inline double uniform_real(double a, double b) {
            return this->uniform_real_rng_(this->engine_,
                    typename decltype(this->uniform_real_rng_)::param_type (a, b));
        }

        // returns an integer in {0, 1} sampled from a Bernoulli distribution with probability of 1
        // given by p
        inline bool bernoulli(double p=0.5) {
            return this->bernoulli_rng_(this->engine_,
                    typename decltype(this->bernoulli_rng_)::param_type (p));
        }

        // returns a binomial random variate with probability of success in n trials given by p
        inline unsigned long binomial(double p, unsigned long n) {
            return this->binomial_rng_(this->engine_,
                    typename decltype(this->binomial_rng_)::param_type (p, n));
        }

        // returns a random variate from an exponential distribution with rate parameter p
        inline double exponential(double p) {
            if (p == 0) {
                return 0.0;
            }
            return this->exponential_rng_(this->engine_,
                    typename decltype(this->exponential_rng_)::param_type (p));
        }

        // returns a random variate from normal distribution with given mean and standard deviation
        inline double normal(double mean, double stddev) {
            return this->normal_rng_(this->engine_,
                    typename decltype(this->normal_rng_)::param_type (mean, stddev));
        }

        // returns an integer sampled from a Geometric distribution with rate parameter p,
        // with support in {0, 1, 2, ...}
        inline unsigned long geometric(double p) {
            return this->geometric_rng_(this->engine_,
                    typename decltype(this->geometric_rng_)::param_type (p));
        }

        // returns an integer value sampled from a Poisson distribution with mean mu
        inline unsigned long poisson(double mu) {
            if (mu <= 0) {
                return 0;
            }
            return this->poisson_rng_(this->engine_,
                    typename decltype(this->poisson_rng_)::param_type (mu));
        }

    private:
        RandomSeedType                                          seed_;
        EngineT                                                 engine_;
        std::uniform_real_distribution<double>                  uniform_real_rng_;
        std::uniform_int_distribution<long>                     uniform_int_rng_;
        std::uniform_int_distribution<unsigned long>            uniform_pos_int_rng_;
        std::bernoulli_distribution                             bernoulli_rng_;
        std::binomial_distribution<unsigned long>               binomial_rng_;
        std::exponential_distribution<double>                   exponential_rng_;
        std::normal_distribution<double>                        normal_rng_;
        std::poisson_distribution<unsigned long>                poisson_rng_;
        std::geometric_distribution<unsigned long>              geometric_rng_;

}; // RandomNumberGeneratorTemplate

////////////////////////////////////////////////////////////////////////////////
// RandomNumberGenerator

/**
 * Default random number generator engine.
 */
class RandomNumberGenerator : public RandomNumberGeneratorTemplate<std::mt19937_64> {
}; // RandomNumberGenerator

} // namespace numeric
} // namespace platypus

#endif

