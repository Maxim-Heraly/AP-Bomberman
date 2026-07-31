#pragma once

#include <chrono>

namespace bomberman::logic {

/**
 * @brief Singleton (see section 3.1.1) that tracks the time (in seconds)
 * between two consecutive update steps ("deltaTime"). Fully implemented -
 * you should not need to change this class: call tick() once per frame from
 * Game's main loop, and getDeltaTime() everywhere you need to scale
 * movement/timers so the game runs at the same speed on any device.
 *
 * Uses std::chrono::steady_clock (never system_clock, which can jump when
 * the system time changes) instead of sf::Clock, as required in section 3.1.
 */
class Stopwatch {
public:
    static Stopwatch& getInstance() {
        static Stopwatch instance;
        return instance;
    }

    Stopwatch(const Stopwatch&) = delete;
    Stopwatch& operator=(const Stopwatch&) = delete;

    /// Call exactly once per frame, at the very start of the game loop.
    void tick() {
        const auto now = Clock::now();
        deltaTime = std::chrono::duration<float>(now - lastTick).count();
        lastTick = now;
    }

    /// Seconds elapsed since the previous tick() call.
    float getDeltaTime() const { return deltaTime; }

private:
    using Clock = std::chrono::steady_clock;

    Stopwatch() : lastTick(Clock::now()) {}

    Clock::time_point lastTick;
    float deltaTime{0.f};
};

} // namespace bomberman::logic
