#pragma once

#include "logic/entities/EntityModel.hpp"
#include <array>
#include <memory>

namespace bomberman::logic {

class Character;

class Bomb : public EntityModel {
public:
    struct BlastProfile {
        std::array<int, 4> reach{0, 0, 0, 0}; // Up, Down, Left, Right
        std::array<bool, 4> hasEnd{false, false, false, false};
    };

    /// @brief Creates a bomb at the given position with an owner and explosion radius.
    Bomb(Vector2 position, Vector2 size, std::weak_ptr<Character> owner, int radius);

    /// @brief Updates the bomb's fuse and detonates it when the fuse expires.
    void update(float deltaTime) override;

    /// @brief Returns the maximum number of tiles the explosion can reach.
    [[nodiscard]] int getRadius() const { return radius; }

    /// @brief Returns whether the bomb has already exploded.
    [[nodiscard]] bool hasExploded() const { return exploded; }

    /// @brief Returns a weak reference to the Character that placed the bomb.
    [[nodiscard]] std::weak_ptr<Character> getOwner() const { return owner; }

    /// @brief Returns whether the bomb's owner can still pass through the bomb.
    [[nodiscard]] bool canOwnerPassThrough() const { return ownerCanPassThrough; }

    /// @brief Prevents the bomb's owner from passing through it.
    void disableOwnerPassThrough() { ownerCanPassThrough = false; }

    /// @brief Returns the calculated reach and stopping points of the explosion.
    [[nodiscard]] const BlastProfile& getBlastProfile() const { return blastProfile; }

    /// @brief Stores the calculated explosion blast profile.
    void setBlastProfile(const BlastProfile& profile) { blastProfile = profile; }

    /// @brief Causes the bomb to detonate on its next update.
    void detonateEarly() { fuseRemaining = 0.f; }

private:
    /// @brief Character that placed this bomb, without owning the Character.
    std::weak_ptr<Character> owner;

    /// @brief Maximum number of tiles the explosion can travel in each direction.
    int radius;

    /// @brief Time remaining before the bomb detonates, in seconds.
    float fuseRemaining{2.f};

    /// @brief Indicates whether the bomb has already detonated.
    bool exploded{false};

    /// @brief Indicates whether the bomb's owner can still pass through it.
    bool ownerCanPassThrough{true};

    /// @brief Stores the calculated reach and end points of the explosion.
    BlastProfile blastProfile;
};

} // namespace bomberman::logic