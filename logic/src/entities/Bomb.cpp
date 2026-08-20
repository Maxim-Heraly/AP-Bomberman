#include "logic/entities/Bomb.hpp"
#include "logic/entities/Character.hpp"

namespace bomberman::logic {

    Bomb::Bomb(const Vector2 position, const Vector2 size, std::weak_ptr<Character> owner, const int radius)
        : EntityModel(position, size), owner(std::move(owner)), radius(radius) {}

    void Bomb::update(float deltaTime) {
        if (exploded || !alive) return;

        fuseRemaining -= deltaTime;
        if (fuseRemaining > 0.f) return;

        exploded = true;
        if (auto ownerPtr = owner.lock()) {
            ownerPtr->onBombExploded();
        }

        markDead();
        notify(EventType::BombExploded);
    }

} // namespace bomberman::logic
