#pragma once

#include "logic/entities/Character.hpp"
#include "logic/Score.hpp"

namespace bomberman::logic {

/// The human-controlled Character. Movement input arrives purely via
/// Character::setMovementInput() - translated from arrow-key presses by
/// Game/PlayState - so this class itself stays input-library-agnostic.
class Player : public Character {
public:
    Player(Vector2 position, Vector2 size) : Character(position, size) {}

    std::shared_ptr<Score> getScore() const { return score; }

private:
    std::shared_ptr<Score> score;
    // TODO: Player-specific stat tracking for Score (blocks broken, powerups
    // collected, enemies killed, time alive) can live here, or entirely
    // inside Score via the Observer events - your choice, document the
    // trade-off in your report (section 4.2).
};

} // namespace bomberman::logic
