#pragma once

#include "logic/entities/EntityModel.hpp"

namespace bomberman::logic {

    class Character;

    /**
     * @brief Abstract base class for all power-ups.
     *
     * A PowerUp is a static EntityModel that remains in the world until
     * collected by a Character. Concrete power-ups inherit from this class
     * and implement applyEffect() to define their specific effect.
     */
    class PowerUp : public EntityModel {
    public:
        /**
         * @brief Creates a power-up at the given position and size.
         *
         * @param position The position of the power-up in the world.
         * @param size The dimensions of the power-up's collision area.
         */
        PowerUp(const Vector2 position, const Vector2 size)
            : EntityModel(position, size) {}

        /**
         * @brief Updates the power-up.
         *
         * Power-ups do not move or perform any periodic logic, so this
         * function intentionally does nothing until the power-up is collected.
         *
         * @param deltaTime Time elapsed since the previous update.
         */
        void update(float /*deltaTime*/) override {}

        /**
         * @brief Applies the power-up's effect to a Character.
         *
         * Each concrete power-up implements this function differently,
         * for example by increasing the character's bomb capacity or
         * blast range.
         *
         * @param character The Character collecting the power-up.
         */
        virtual void applyEffect(Character& character) = 0;

        /**
         * @brief Removes the power-up after it has been collected.
         *
         * Marks the power-up as dead so that it can be removed from the
         * World and notifies observers that the power-up was collected.
         */
        void remove() {
            markDead();
            notify(EventType::PowerUpCollected);
        }
    };

} // namespace bomberman::logic