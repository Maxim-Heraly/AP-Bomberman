#include "logic/entities/SkatesPowerUp.hpp"
#include "logic/entities/Character.hpp"

namespace bomberman::logic {

void SkatesPowerUp::applyEffect(Character& character) {
    character.increaseSpeed(0.1f);
    markDead();
    notify(EventType::PowerUpCollected);
}

} // namespace bomberman::logic
