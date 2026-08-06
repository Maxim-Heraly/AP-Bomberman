#pragma once

#include "logic/entities/EntityModel.hpp"
#include "logic/utils/Direction.hpp"
#include <vector>

namespace bomberman::logic {

/**
 * @brief Shared base for the human Player and the computer-controlled Bots
 * (see Player.hpp / Bot.hpp). Holds every stat a power-up can influence.
 *
 * TODO: implement update() - apply movementInput_ (scaled by speed_ *
 * deltaTime) to position_, update facing_, notify(EventType::Moved).
 * Blocking movement against Walls/Bombs is World's job (section 2.1,
 * "Player Controls & Interactions with Objects") - decide in your report
 * whether you resolve that by having World revert position_ after the fact,
 * or by querying World before moving.
 */

class Bomb;

class Character : public EntityModel {
public:
    Character(Vector2 position, Vector2 size);

    void update(float deltaTime) override;


    [[nodiscard]] bool canPlaceBomb() const;

    bool tryPlaceBomb() {
        if (!canPlaceBomb()) return false;
        ++bombsPlaced;
        return true;
    }

    void onBombExploded() {
        if (bombsPlaced > 0) --bombsPlaced;
    }

    void revertToPreviousPosition() { position = previousPosition; }

    void die() {
        if (!alive) return;
        markDead();
        notify(EventType::Died);
    }

    // --- Power-up hooks -------------------------------------------------
    void increaseBombRadius(int amount) { bombRadius += amount; }
    void increaseMaxBombs(int amount) { maxBombs += amount; }
    void increaseSpeed(float amount) { speed += amount; }

    [[nodiscard]] int getBombRadius() const { return bombRadius; }
    [[nodiscard]] int getMaxBombs() const { return maxBombs; }
    [[nodiscard]] float getSpeed() const { return speed; }
    [[nodiscard]] Direction getFacing() const { return facing; }
    [[nodiscard]] bool isMoving() const { return movementInput != Direction::None; }

    /// Called each frame by Game/PlayState (for the Player, translated from
    /// arrow-key presses) or by the bot AI (for a Bot) to queue up a
    /// movement direction for the next update().
    void setMovementInput(Direction direction) { movementInput = direction; }

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
