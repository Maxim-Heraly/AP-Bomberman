#pragma once

#include "logic/entities/EntityModel.hpp"

namespace bomberman::logic {

    /// Represents an indestructible or destructible arena block.
    /// World handles the destruction effects, such as spawning a PowerUp.
    class Wall : public EntityModel {
    public:
        /// Creates a wall at the given position and size with its destruction
        /// behaviour determined by whether it is destructible.
        Wall(const Vector2 position, const Vector2 size, const bool destructible)
            : EntityModel(position, size), destructible(destructible) {}

        /// Walls are static, so no per-frame update is required.
        void update(float /*deltaTime*/) override {}

        /// Returns whether this wall can be destroyed by a bomb.
        [[nodiscard]] bool isDestructible() const { return destructible; }

        /// Marks the wall as dead and notifies observers that it was destroyed.
        void destroy();

    private:
        /// Indicates whether bombs are allowed to destroy this wall.
        bool destructible;
    };

} // namespace bomberman::logic