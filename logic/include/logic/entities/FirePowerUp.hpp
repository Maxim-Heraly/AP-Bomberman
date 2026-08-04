#pragma once

#include "logic/entities/PowerUp.hpp"

namespace bomberman::logic {

/// Increases the collecting Character's bomb blast radius by one.
class FirePowerUp : public PowerUp {
public:
    using PowerUp::PowerUp;

    void applyEffect(Character& character) override;
};

} // namespace bomberman::logic
