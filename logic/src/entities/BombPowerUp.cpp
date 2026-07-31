#include "logic/entities/BombPowerUp.hpp"
#include "logic/entities/Character.hpp"

namespace bomberman::logic {

void BombPowerUp::applyEffect(Character& character) {
    // TODO: character.increaseMaxBombs(1); markDead(); notify(EventType::PowerUpCollected);
    (void)character;
}

} // namespace bomberman::logic
