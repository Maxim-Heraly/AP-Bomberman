#include "logic/entities/FirePowerUp.hpp"
#include "logic/entities/Character.hpp"

namespace bomberman::logic {

void FirePowerUp::applyEffect(Character& character) {
    // TODO: character.increaseBombRadius(1); markDead(); notify(EventType::PowerUpCollected);
    (void)character;
}

} // namespace bomberman::logic
