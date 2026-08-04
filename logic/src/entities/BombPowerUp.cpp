#include "logic/entities/BombPowerUp.hpp"
#include "logic/entities/Character.hpp"

namespace bomberman::logic {

void BombPowerUp::applyEffect(Character& character) {
    character.increaseMaxBombs(1);
    markDead();
    notify(EventType::PowerUpCollected);
}

} // namespace bomberman::logic
