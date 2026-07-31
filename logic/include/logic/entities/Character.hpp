#pragma once

#include "logic/entities/EntityModel.hpp"
#include "logic/utils/Direction.hpp"

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
class Character : public EntityModel {
public:
    Character(Vector2 position, Vector2 size);

    void update(float deltaTime) override; // TODO

    /// TODO: World should check this before creating a Bomb via the factory.
    bool canPlaceBomb() const;

    // --- Power-up hooks -------------------------------------------------
    // TODO: call these from FirePowerUp/BombPowerUp/SkatesPowerUp::applyEffect().
    void increaseBombRadius(int amount) { bombRadius += amount; }
    void increaseMaxBombs(int amount) { maxBombs += amount; }
    void increaseSpeed(float amount) { speed += amount; }

    int getBombRadius() const { return bombRadius; }
    int getMaxBombs() const { return maxBombs; }
    float getSpeed() const { return speed; }
    Direction getFacing() const { return facing; }

    /// Called each frame by Game/PlayState (for the Player, translated from
    /// arrow-key presses) or by the bot AI (for a Bot) to queue up a
    /// movement direction for the next update().
    void setMovementInput(Direction direction) { movementInput = direction; }

protected:
    float speed{0.5f};      // TODO: tune starting value - world coords span [-1, 1], so keep this small.
    int bombRadius{1};       // Starting radius per section 2.1: "one bomb ... with a radius of one".
    int maxBombs{1};
    int bombsPlaced{0};       // TODO: increment on placeBomb, decrement when that bomb explodes.
    Direction facing{Direction::Down};
    Direction movementInput{Direction::None};
};

} // namespace bomberman::logic
