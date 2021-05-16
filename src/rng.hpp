#pragma once
#include <random>

namespace radl {

class rng_t {
private:
    std::mt19937_64 m_rng;
    uint_fast64_t m_seed;

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
     * @brief Computs results in the form of DnD dice rolls.
     *
     * e.g.: 2d6, the output will be in closed interval [2, 12] for this
     * example.
     *
     * @param n
     * @param d
     * @return int
     */
    inline int roll_dice(int n, int d) {
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
     * @tparam InIter
     * @tparam OutIter
     * @param first
     * @param last
     * @param output
     * @return true
     * @return false
     */
    template <typename InIter, typename OutIter>
    bool random_choice(InIter first, InIter last, OutIter& output) {
        // if empty
        if(first == last) {
            return false;
        }
        std::sample(first, last, output, 1, this->m_rng);
        return true;
    }
};

}  // namespace radl
