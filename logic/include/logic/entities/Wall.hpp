#pragma once

#include "logic/entities/EntityModel.hpp"

namespace bomberman::logic {

/// Indestructible or destructible arena block. Mostly a data holder - the
/// interesting logic (spawning a powerup when destroyed) lives in World,
/// since it needs the AbstractFactory to create the PowerUp.
class Wall : public EntityModel {
public:
    Wall(Vector2 position, Vector2 size, bool destructible)
        : EntityModel(position, size), destructible(destructible) {}

    void update(float /*deltaTime*/) override {} // Walls are static, nothing to tick.

    [[nodiscard]] bool isDestructible() const { return destructible; }

    void destroy();

private:
    bool destructible;
};

} // namespace bomberman::logic
