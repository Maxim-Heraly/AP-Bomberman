#include "logic/entities/Character.hpp"
#include "logic/utils/Direction.hpp"

namespace bomberman::logic {

Character::Character(Vector2 position, Vector2 size) : EntityModel(position, size) {}

void Character::update(float deltaTime) {
    previousPosition = position;
    const Vector2 movement = directionToVector(movementInput);
    if (movement.x == 0.f && movement.y == 0.f) return;
    position += movement * speed * deltaTime;
    facing = movementInput;
    notify(EventType::Moved);
}

bool Character::canPlaceBomb() const {
    return bombsPlaced < maxBombs;
}

} // namespace bomberman::logic
