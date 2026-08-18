#include "logic/entities/EntityModel.hpp"
#include <cmath>

namespace bomberman::logic {

bool EntityModel::intersects(const EntityModel& other) const {
    const bool overlapX = std::abs(position.x - other.position.x) < (hitbox.x + other.hitbox.x) * 0.5f;
    const bool overlapY = std::abs(position.y - other.position.y) < (hitbox.y + other.hitbox.y) * 0.5f;
    return overlapX && overlapY;
}

} // namespace bomberman::logic
