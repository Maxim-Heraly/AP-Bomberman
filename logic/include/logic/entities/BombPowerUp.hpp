#pragma once

#include "logic/entities/PowerUp.hpp"

namespace bomberman::logic {

/// Increases the collecting Character's number of simultaneously placeable
/// bombs by one
class BombPowerUp : public PowerUp {
public:
    using PowerUp::PowerUp;

    /// Increases the Character's maximum number of active bombs by one,
    /// then removes this power-up and notifies observers that it was collected.
    void applyEffect(Character& character) override;
};

} // namespace bomberman::logic
