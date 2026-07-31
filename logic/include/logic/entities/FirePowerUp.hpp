#pragma once

#include "logic/entities/PowerUp.hpp"

namespace bomberman::logic {

/// Increases the collecting Character's bomb blast radius by one (section 2.1).
class FirePowerUp : public PowerUp {
public:
    using PowerUp::PowerUp;

    void applyEffect(Character& character) override; // TODO
};

} // namespace bomberman::logic
