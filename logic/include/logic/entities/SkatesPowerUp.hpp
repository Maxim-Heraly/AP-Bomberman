#pragma once

#include "logic/entities/PowerUp.hpp"

namespace bomberman::logic {

/// Increases the collecting Character's movement speed (section 2.1, "Skates").
class SkatesPowerUp : public PowerUp {
public:
    using PowerUp::PowerUp;

    void applyEffect(Character& character) override; // TODO
};

} // namespace bomberman::logic
