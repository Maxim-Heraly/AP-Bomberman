#include "logic/entities/FirePowerUp.hpp"
#include "logic/entities/Character.hpp"

namespace bomberman::logic {

void FirePowerUp::applyEffect(Character& character) {
    character.increaseBombRadius(1);
    markDead();
    notify(EventType::PowerUpCollected);
}

} // namespace bomberman::logic
