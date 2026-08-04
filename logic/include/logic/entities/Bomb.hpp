#pragma once

#include "logic/entities/EntityModel.hpp"
#include <memory>
#include <array>

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
    struct BlastProfile {
        std::array<int, 4> reach{0, 0, 0, 0}; // Up, Down, Left, Right
        std::array<bool, 4> hasEnd{false, false, false, false};
    };
    Bomb(Vector2 position, Vector2 size, std::weak_ptr<Character> owner, int radius);

    void update(float deltaTime) override;

    [[nodiscard]] int getRadius() const { return radius; }
    [[nodiscard]] bool hasExploded() const { return exploded; }
    [[nodiscard]] std::weak_ptr<Character> getOwner() const { return owner; }

    [[nodiscard]] bool canOwnerPassThrough() const { return ownerCanPassThrough; }
    void disableOwnerPassThrough() { ownerCanPassThrough = false; }

    [[nodiscard]] const BlastProfile& getBlastProfile() const { return blastProfile; }
    void setBlastProfile(BlastProfile profile) { blastProfile = profile; }

    void detonateEarly() { fuseRemaining = 0.f; }

private:
    std::weak_ptr<Character> owner;
    int radius;
    float fuseRemaining{3.f};
    bool exploded{false};
    bool ownerCanPassThrough{true};
    BlastProfile blastProfile;
};

} // namespace bomberman::logic
