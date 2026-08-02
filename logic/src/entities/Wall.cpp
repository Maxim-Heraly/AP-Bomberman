#include "logic/entities/Wall.hpp"

namespace bomberman::logic {

void Wall::destroy() {
    markDead();
    notify(EventType::BlockDestroyed);
}

} // namespace bomberman::logic
