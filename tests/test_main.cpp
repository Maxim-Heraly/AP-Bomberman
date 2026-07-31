// Minimal, dependency-free test harness (no unit-testing framework was
// taught for this course, see section 3.2 "Write small tests for your game
// logic"). Add one function per behaviour you want to guard against
// regressions, call it from main(), and use the ASSERT macro below - it
// prints a clear message and exits non-zero on the first failure, which
// CTest (and your CI, see .circleci/config.yml) interprets as a failing test.

#include <cstdlib>
#include <iostream>

#define ASSERT(condition, message)                                                   \
    if (!(condition)) {                                                              \
        std::cerr << "FAILED: " << message << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        std::exit(1);                                                                \
    }

// TODO: #include the logic headers you're testing, e.g. "logic/entities/Wall.hpp"

void testEntityModelIntersects() {
    // TODO: construct two overlapping and two non-overlapping EntityModel
    // instances (or a trivial subclass, since EntityModel is abstract) and
    // ASSERT that intersects() returns the right answer for both cases.
}

void testBombFuseCountsDown() {
    // TODO: construct a Bomb, call update() with a large enough deltaTime
    // (or several small calls), ASSERT hasExploded() becomes true.
}

void testScoring() {
    // TODO: construct a Score, feed it a few onNotify() calls, ASSERT
    // getCurrentScore() matches what you'd expect (section 2.1 "Scoring").
}

int main() {
    testEntityModelIntersects();
    testBombFuseCountsDown();
    testScoring();
    std::cout << "All tests passed.\n";
    return 0;
}
