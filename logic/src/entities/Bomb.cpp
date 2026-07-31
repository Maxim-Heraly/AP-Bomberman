#include "logic/entities/Bomb.hpp"

namespace bomberman::logic {

Bomb::Bomb(Vector2 position, Vector2 size, std::weak_ptr<Character> owner, int radius)
    : EntityModel(position, size), owner(std::move(owner)), radius(radius) {}

void Bomb::update(float deltaTime) {
    // TODO: count fuseRemaining_ down by deltaTime. Once it reaches <= 0:
    // set exploded_ = true, markDead(), notify(EventType::BombExploded).
    // The actual cross-shaped destruction happens in World::explode(),
    // triggered from World::update() once it sees hasExploded() == true.
    (void)deltaTime;
}

} // namespace bomberman::logic
