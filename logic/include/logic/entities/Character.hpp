#pragma once

#include "logic/entities/EntityModel.hpp"
#include "logic/utils/Direction.hpp"

namespace bomberman::logic {

/**
 * @brief Shared base for the human Player and the computer-controlled Bots
 * (see Player.hpp / Bot.hpp). Holds every stat a power-up can influence.
 *
 */

class Bomb;

class Character : public EntityModel {
public:
    Character(Vector2 position, Vector2 size);

    void update(float deltaTime) override;

    [[nodiscard]] bool canPlaceBomb() const;

    bool tryPlaceBomb() {
        if (!canPlaceBomb())
            return false;
        ++bombsPlaced;
        return true;
    }

    void onBombExploded() {
        if (bombsPlaced > 0)
            --bombsPlaced;
    }

    void revertToPreviousPosition() { position = previousPosition; }
    [[nodiscard]] const Vector2& getPreviousPosition() const { return previousPosition; }

    void die() {
        if (!alive)
            return;
        markDead();
        notify(EventType::Died);
    }

    // --- Power-up hooks -------------------------------------------------
    void increaseBombRadius(const int amount) { bombRadius += amount; }
    void increaseMaxBombs(const int amount) { maxBombs += amount; }
    void increaseSpeed(const float amount) { speed += amount; }

    [[nodiscard]] int getBombRadius() const { return bombRadius; }
    [[nodiscard]] int getMaxBombs() const { return maxBombs; }
    [[nodiscard]] float getSpeed() const { return speed; }
    [[nodiscard]] Direction getFacing() const { return facing; }
    [[nodiscard]] bool isMoving() const { return movementInput != Direction::None; }

    /// Called each frame by Game/PlayState (for the Player, translated from
    /// arrow-key presses) or by the bot AI (for a Bot) to queue up a
    /// movement direction for the next update().
    void setMovementInput(const Direction direction) { movementInput = direction; }

    void declareEnemyKilled() { notify(EventType::EnemyKilled); }
    void collectPowerUp() { notify(EventType::PlayerCollectedPowerUp); }
    void declareBlockDestroyed() { notify(EventType::PlayerDestroyedBlock); }
    void declareWin() { notify(EventType::PlayerWon); }
    void declareLoss() { notify(EventType::PlayerLost); }

protected:
    float speed{0.5f};
    int bombRadius{1};
    int maxBombs{1};
    int bombsPlaced{0};
    Direction facing{Direction::Down};
    Direction movementInput{Direction::None};
    Vector2 previousPosition{};
};

} // namespace bomberman::logic
