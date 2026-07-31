#pragma once

#include "logic/entities/EntityModel.hpp"

namespace bomberman::logic {

class Character;

/// Abstract base for the three required power-ups (section 2.1,
/// "Power-ups"). Subclasses only need to implement applyEffect().
class PowerUp : public EntityModel {
public:
    PowerUp(Vector2 position, Vector2 size) : EntityModel(position, size) {}

    void update(float /*deltaTime*/) override {} // Power-ups are static until collected.

    /// TODO: World calls this when a Character walks over this PowerUp.
    /// Implementations should call the relevant Character::increaseXxx(),
    /// markDead(), and notify(EventType::PowerUpCollected).
    virtual void applyEffect(Character& character) = 0;
};

} // namespace bomberman::logic
