#pragma once

#include <random>

namespace bomberman::logic {

/**
 * @brief Singleton (see section 3.1.1) wrapping a single std::mt19937
 * engine. Fully implemented - reuse this everywhere you need randomness
 * (powerup drop chance, arena generation, bot decision making, ...) instead
 * of creating new generators or using the legacy rand()/srand().
 */
class Random {
public:
    static Random& getInstance() {
        static Random instance;
        return instance;
    }

    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    /// Random integer in [min, max], inclusive.
    int getInt(int min, int max) {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine);
    }

    /// Random float in [min, max).
    float getFloat(float min, float max) {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine);
    }

    /// Returns true with the given probability (0.0 - 1.0), e.g. useful for
    /// "25% chance of spawning a powerup" (section 2.1, Power-ups).
    bool chance(float probability) {
        return getFloat(0.f, 1.f) < probability;
    }

private:
    Random() : engine(std::random_device{}()) {}

    std::mt19937 engine;
};

} // namespace bomberman::logic
