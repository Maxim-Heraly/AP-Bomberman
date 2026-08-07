#pragma once

#include "logic/utils/Vector2.hpp"

namespace bomberman::logic {

/// Facing/movement direction, shared between Character (logic) and
/// CharacterView (representation) so the View knows which walk animation to play.
enum class Direction { Up, Down, Left, Right, None };

/// Convenience helper: converts a Direction into a unit
/// vector you can scale by speed * deltaTime, e.g. in Character::update().
inline Vector2 directionToVector(Direction direction) {
    switch (direction) {
        case Direction::Up:    return {0.f, -1.f};
        case Direction::Down:  return {0.f, 1.f};
        case Direction::Left:  return {-1.f, 0.f};
        case Direction::Right: return {1.f, 0.f};
        case Direction::None:  return {0.f, 0.f};
    }
    return {0.f, 0.f};
}

} // namespace bomberman::logic
