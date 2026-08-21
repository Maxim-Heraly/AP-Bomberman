#pragma once

#include "logic/entities/PowerUp.hpp"

namespace bomberman::logic {

/// Increases the collecting Character's movement speed.
class SkatesPowerUp : public PowerUp {
public:
    using PowerUp::PowerUp;

    /// Increases the Character's movement speed,
    /// then removes this power-up and notifies observers that it was collected.
    void applyEffect(Character& character) override;
};

} // namespace bomberman::logic
