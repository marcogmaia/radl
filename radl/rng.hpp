#pragma once

#include <algorithm>
#include <random>

namespace radl {

class rng_t {
private:
    std::mt19937_64 m_rng;
    uint_fast64_t m_seed;

    std::uniform_real_distribution<double> m_real_distribuition1
        = std::uniform_real_distribution<double>(0.0, 1.0);

public:
    /**
     * @brief Constructs the rng with the actual time as seed
     */
    rng_t() {
        m_seed = time(nullptr);
        m_rng  = std::mt19937_64(m_seed);
    }

    /**
     * @brief Constructs the rng with specific seed
     *
     * @param seed
     */
    inline explicit rng_t(int seed) {
        m_seed = seed;
        m_rng  = std::mt19937_64(seed);
    }

    /**
     * @brief Constructs the rng with a string as seed
     *
     * @param seed string
     */
    inline explicit rng_t(const std::string& seed) {
        auto hsseed = std::hash<std::string>{}(seed);
        m_rng       = std::mt19937_64(hsseed);
    }

    /**
     * @brief Get a random number in the closed interval [min, max]
     *
     * @param min
     * @param max
     * @return int
     */
    inline int range(int min, int max) {
        std::uniform_int_distribution<int> udist(min, max);
        return udist(m_rng);
    }

    /**
     * @brief Computes results in the form of DnD dice rolls.
     *
     * e.g.: 2d6, the output will be in closed interval [2, 12] for this
     * example.
     *
     * @param n
     * @param d
     * @return total sum of the dices
     */
    inline int dice_roll(int n, int d) {
        int total = 0;
        for(; n; --n) {
            total += range(1, d);
        }
        return total;
    }

    /**
     * @brief Picks a random value in range [first - last] iterators,  and puts
     * in "output"
     *
     * @tparam InputIterator input iterator
     * @tparam OutputIterator output iterator
     * @param first
     * @param last
     * @param output
     *
     * @return true if sampled, false otherwise
     */
    template <typename InputIterator, typename OutputIterator>
    bool random_choice(InputIterator first, InputIterator last,
                       OutputIterator output) {
        // if empty
        if(first == last) {
            return false;
        }
        std::sample(first, last, output, 1, this->m_rng);
        return true;
    }

    /**
     * @brief Get a random double in the open inteval [0.0, 1.0)
     *
     * @return double
     */
    inline double random_double() {
        return m_real_distribuition1(m_rng);
    }
};

}  // namespace radl
