#pragma once

#include "logic/entities/Character.hpp"

namespace bomberman::logic {

class World; // Forward declaration only - see decideNextMove().

/**
 * @brief Computer-controlled Character. TODO: implement the required AI
 * (section 2.1, "Enemies and AI"):
 *   1. Basic survival: if a live Bomb's blast would reach this Bot, flee.
 *   2. Move towards and collect nearby PowerUps.
 *   3. Regularly place bombs next to destructible Walls.
 *   4. Once no destructible walls remain (or an enemy is close), target
 *      other Characters with bombs.
 *   5. Take bombRadius_/maxBombs_ into account (flee further if radius is
 *      bigger, place multiple bombs at once if maxBombs_ > 1).
 *
 * Suggestion for the "smarter bots" bonus (section 2.3): make this class
 * abstract (pure virtual decideNextMove()) and create several concrete
 * subclasses, each with a different personality/strategy.
 */
class Bot : public Character {
public:
    Bot(Vector2 position, Vector2 size) : Character(position, size) {}

    /// TODO: called once per tick (by World::update(), before update()) -
    /// inspect the World (nearby bombs/walls/powerups/characters) and call
    /// setMovementInput(...) / request a bomb placement accordingly.
    void decideNextMove(const World& world);
};

} // namespace bomberman::logic
