#pragma once

#include "logic/entities/Character.hpp"

namespace bomberman::logic {

/// The human-controlled Character. Movement input arrives purely via
/// Character::setMovementInput() - translated from arrow-key presses by
/// Game/PlayState - so this class itself stays input-library-agnostic.
class Player : public Character {
public:
    Player(Vector2 position, Vector2 size) : Character(position, size) {}

};

} // namespace bomberman::logic
