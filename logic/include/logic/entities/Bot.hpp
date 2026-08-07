#pragma once

#include "logic/entities/Character.hpp"

namespace bomberman::logic {

    class World; // Forward declaration only - see decideNextMove().

    /**
     * @brief Computer-controlled Character behaviour: a priority list
     * re-evaluated every tick by decideNextMove().
     *
     *   1. Flee any Bomb whose blast would currently reach this Bot.
     *   2. Chase down any nearby PowerUp.
     *   3. If an enemy Character is very close, attack instead of detouring
     *      to a wall.
     *   4. Otherwise, hunt down and bomb the nearest destructible Wall.
     *   5. Once no destructible Walls are left (or nothing else applies),
     *      fall back to attacking the nearest Character.
     *
     * Each helper below returns true once it has decided on an action for
     * this tick, so decideNextMove() just chains them in priority order.
     */
    class Bot : public Character {
    public:
        Bot(Vector2 position, Vector2 size) : Character(position, size) {}

        /// Called once per tick (by World::update(), before update()) -
        /// inspects the World and calls setMovementInput()/requests a bomb
        /// placement accordingly.
        void decideNextMove(World& world);

    private:
        bool tryFlee(World& world);
        bool tryCollectPowerUp(World& world);
        bool tryBreakWalls(World& world);
        bool tryAttack(World& world);

        [[nodiscard]] bool isEnemyClose(const World& world) const;
        [[nodiscard]] bool isTileDangerous(const World& world, const Vector2& tile) const;
        [[nodiscard]] bool isTileBlocked(const World& world, const Vector2& tile) const;
        [[nodiscard]] bool isInLineWithinRange(const Vector2& from, const Vector2& target, int range) const;
        [[nodiscard]] Direction directionTowards(const Vector2& target) const;
    };

} // namespace bomberman::logic