#pragma once

#include "logic/entities/PowerUp.hpp"

namespace bomberman::logic {

/// Increases the collecting Character's number of simultaneously placeable
/// bombs by one (section 2.1, "Extra Bomb").
class BombPowerUp : public PowerUp {
public:
    using PowerUp::PowerUp;

    void applyEffect(Character& character) override; // TODO
};

} // namespace bomberman::logic
