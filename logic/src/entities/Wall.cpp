#include "logic/entities/Wall.hpp"

namespace bomberman::logic {

void Wall::destroy() {
    // TODO: markDead() and notify(EventType::BlockDestroyed). Remember:
    // World is responsible for then rolling the dice on spawning a PowerUp
    // at this position (section 2.1, "Power-ups").
}

} // namespace bomberman::logic
