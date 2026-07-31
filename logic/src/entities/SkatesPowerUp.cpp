#include "logic/entities/SkatesPowerUp.hpp"
#include "logic/entities/Character.hpp"

namespace bomberman::logic {

void SkatesPowerUp::applyEffect(Character& character) {
    // TODO: character.increaseSpeed(...); markDead(); notify(EventType::PowerUpCollected);
    (void)character;
}

} // namespace bomberman::logic
