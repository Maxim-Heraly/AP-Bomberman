#include "logic/entities/Character.hpp"
#include "logic/utils/Direction.hpp"

namespace bomberman::logic {

// Constants for hitbox scaling to make the character's hitbox slightly smaller than its visual representation to get
// around corners a bit easier.
namespace {
constexpr float kHitboxScale = 0.9f;
}

Character::Character(const Vector2 position, const Vector2 size) : EntityModel(position, size, size * kHitboxScale) {}

void Character::update(const float deltaTime) {
    previousPosition = position;
    const Vector2 movement = directionToVector(movementInput);
    if (movement.x == 0.f && movement.y == 0.f)
        return;
    position += movement * speed * deltaTime;
    facing = movementInput;
    notify(EventType::Moved);
}

bool Character::canPlaceBomb() const { return bombsPlaced < maxBombs; }

} // namespace bomberman::logic
