#pragma once

#include "logic/entities/PowerUp.hpp"

namespace bomberman::logic {

/// Increases the collecting Character's bomb blast radius by one.
class FirePowerUp : public PowerUp {
public:
    using PowerUp::PowerUp;

    /// Increases the Character's bombs blast radius by one,
    /// then removes this power-up and notifies observers that it was collected.
    void applyEffect(Character& character) override;
};

} // namespace bomberman::logic
