#pragma once

#include "logic/entities/EntityModel.hpp"
#include "logic/utils/Direction.hpp"

namespace bomberman::logic {

/**
 * @brief Shared base class for the human Player and computer-controlled Bots.
 *
 * Stores the character statistics and movement state that can be modified by
 * power-ups or used by the character's movement and bomb logic.
 */
class Bomb;

class Character : public EntityModel {
public:
    /**
     * @brief Creates a character at the given position and size.
     *
     * The hitbox is slightly smaller than the visual size to make movement
     * and collision handling more forgiving.
     */
    Character(Vector2 position, Vector2 size);

    /**
     * @brief Updates the character's position according to its current movement input.
     *
     * The character moves at its current speed and notifies observers when it moves.
     */
    void update(float deltaTime) override;

    /**
     * @brief Checks whether the character can currently place another bomb.
     *
     * A character can place a bomb while its number of active bombs is below its maximum.
     */
    [[nodiscard]] bool canPlaceBomb() const;

    /**
     * @brief Records that the character has placed a bomb if its bomb limit allows it.
     *
     * Returns true when the bomb can be placed and false otherwise.
     */
    bool tryPlaceBomb() {
        if (!canPlaceBomb())
            return false;
        ++bombsPlaced;
        return true;
    }

    /**
     * @brief Records that one of the character's placed bombs has exploded.
     *
     * Decreases the active bomb count without allowing it to become negative.
     */
    void onBombExploded() {
        if (bombsPlaced > 0)
            --bombsPlaced;
    }

    /**
     * @brief Restores the character's position from before the previous update.
     */
    void revertToPreviousPosition() { position = previousPosition; }

    /**
     * @brief Returns the character's position before the previous movement update.
     */
    [[nodiscard]] const Vector2& getPreviousPosition() const { return previousPosition; }

    /**
     * @brief Marks the character as dead and notifies observers of the death.
     */
    void die() {
        if (!alive)
            return;
        markDead();
        notify(EventType::Died);
    }

    // --- Power-up hooks -------------------------------------------------

    /**
     * @brief Increases the number of tiles covered by the character's bombs.
     */
    void increaseBombRadius(const int amount) { bombRadius += amount; }

    /**
     * @brief Increases the maximum number of bombs the character can have active.
     */
    void increaseMaxBombs(const int amount) { maxBombs += amount; }

    /**
     * @brief Increases the character's movement speed.
     */
    void increaseSpeed(const float amount) { speed += amount; }

    /**
     * @brief Returns the current bomb explosion radius.
     */
    [[nodiscard]] int getBombRadius() const { return bombRadius; }

    /**
     * @brief Returns the maximum number of simultaneously active bombs.
     */
    [[nodiscard]] int getMaxBombs() const { return maxBombs; }

    /**
     * @brief Returns the character's current movement speed.
     */
    [[nodiscard]] float getSpeed() const { return speed; }

    /**
     * @brief Returns the direction the character is currently facing.
     */
    [[nodiscard]] Direction getFacing() const { return facing; }

    /**
     * @brief Returns whether the character currently has a movement direction.
     */
    [[nodiscard]] bool isMoving() const { return movementInput != Direction::None; }

    /**
     * @brief Sets the movement direction that will be used during the next update.
     *
     * Player input and bot AI both use this function to control character movement.
     */
    void setMovementInput(const Direction direction) { movementInput = direction; }

    /**
     * @brief Notifies observers that this character has killed an enemy.
     */
    void declareEnemyKilled() { notify(EventType::EnemyKilled); }

    /**
     * @brief Notifies observers that this character has collected a power-up.
     */
    void collectPowerUp() { notify(EventType::PlayerCollectedPowerUp); }

    /**
     * @brief Notifies observers that this character has destroyed a block.
     */
    void declareBlockDestroyed() { notify(EventType::PlayerDestroyedBlock); }

    /**
     * @brief Notifies observers that this character has won the game.
     */
    void declareWin() { notify(EventType::PlayerWon); }

    /**
     * @brief Notifies observers that this character has lost the game.
     */
    void declareLoss() { notify(EventType::PlayerLost); }

protected:
    /// Movement speed in world units per second.
    float speed{0.5f};

    /// Number of tiles reached by a bomb's explosion in each direction.
    int bombRadius{1};

    /// Maximum number of bombs this character can have active simultaneously.
    int maxBombs{1};

    /// Number of bombs currently placed by this character that have not exploded yet.
    int bombsPlaced{0};

    /// Direction the character is currently facing.
    Direction facing{Direction::Down};

    /// Movement direction requested for the next update.
    Direction movementInput{Direction::None};

    /// Position stored before the most recent movement update, used for reverting movement.
    Vector2 previousPosition{};
};

} // namespace bomberman::logic