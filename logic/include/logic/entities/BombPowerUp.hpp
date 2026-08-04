#pragma once

#include "logic/entities/PowerUp.hpp"

namespace bomberman::logic {

/// Increases the collecting Character's number of simultaneously placeable
/// bombs by one
class BombPowerUp : public PowerUp {
public:
    using PowerUp::PowerUp;

    void applyEffect(Character& character) override;
};

} // namespace bomberman::logic
