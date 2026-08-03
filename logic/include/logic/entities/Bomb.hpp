#pragma once

#include "logic/entities/EntityModel.hpp"
#include <memory>

namespace bomberman::logic {

class Character;

/**
 * @brief TODO: implement update() to count fuseRemaining_ down and, once it
 * reaches zero, notify(EventType::BombExploded) and markDead(). The actual
 * "cross-shaped destruction" logic belongs in World::explode() (it needs to
 * inspect neighbouring Walls/Bombs/Characters) - this class only owns its
 * own timer/radius/owner state (section 2.1, "Bombs and Explosions").
 */
class Bomb : public EntityModel {
public:
    Bomb(Vector2 position, Vector2 size, std::weak_ptr<Character> owner, int radius);

    void update(float deltaTime) override; // TODO: fuse countdown

    int getRadius() const { return radius; }
    bool hasExploded() const { return exploded; }
    std::weak_ptr<Character> getOwner() const { return owner; }
    bool canOwnerPassThrough() const { return ownerCanPassThrough; }
    void disableOwnerPassThrough() { ownerCanPassThrough = false; }

    /// TODO: World calls this when a chain-reaction explosion reaches this
    /// bomb before its own fuse would have run out.
    void detonateEarly() { fuseRemaining = 0.f; }

private:
    std::weak_ptr<Character> owner;
    int radius;
    float fuseRemaining{3.f};
    bool exploded{false};
    bool ownerCanPassThrough{true};
};

} // namespace bomberman::logic
