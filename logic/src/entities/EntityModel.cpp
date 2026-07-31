#include "logic/entities/EntityModel.hpp"
#include <cmath>

namespace bomberman::logic {

bool EntityModel::intersects(const EntityModel& other) const {
    // Basic axis-aligned bounding-box overlap check, in normalized world
    // coordinates. TODO: replace this with something more refined if you
    // need it (e.g. circular colliders for Characters, or per-entity
    // hitboxes smaller than the full tile).
    const bool overlapX = std::abs(position.x - other.position.x) < (size.x + other.size.x) * 0.5f;
    const bool overlapY = std::abs(position.y - other.position.y) < (size.y + other.size.y) * 0.5f;
    return overlapX && overlapY;
}

} // namespace bomberman::logic
