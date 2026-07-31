#include "logic/entities/Character.hpp"

namespace bomberman::logic {

Character::Character(Vector2 position, Vector2 size) : EntityModel(position, size) {}

void Character::update(float deltaTime) {
    // TODO: translate movementInput_ into a position delta
    // (directionToVector(movementInput_) * speed_ * deltaTime), update
    // position_ and facing_ accordingly, then notify(EventType::Moved) if
    // the Character actually moved. Actual collision against Walls/Bombs is
    // resolved by World::handleCollisions() afterwards - decide in your
    // report whether that reverts position_ here or queries World first.
    (void)deltaTime;
}

bool Character::canPlaceBomb() const {
    return bombsPlaced < maxBombs;
}

} // namespace bomberman::logic
