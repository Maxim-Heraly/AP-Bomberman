#pragma once

#include "logic/entities/EntityModel.hpp"

namespace bomberman::logic {

class Character;

/// Abstract base for the power-ups
/// Subclasses only need to implement applyEffect().
class PowerUp : public EntityModel {
public:
    PowerUp(const Vector2 position, const Vector2 size) : EntityModel(position, size) {}

    void update(float /*deltaTime*/) override {} // Power-ups are static until collected.

    virtual void applyEffect(Character& character) = 0;

    void remove() {
        markDead();
        notify(EventType::PowerUpCollected);
    }
};

} // namespace bomberman::logic
