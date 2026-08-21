#pragma once

#include "logic/entities/Character.hpp"
#include <utility>

namespace bomberman::logic {

class World;

/**
 * @brief Computer-controlled Character behaviour: a priority list
 * re-evaluated every tick by decideNextMove().
 *
 *   1. Flee any Bomb whose blast would currently reach this Bot.
 *   2. Chase nearby PowerUps.
 *   3. Hunt and destroy destructible Walls.
 *   4. Attack the nearest Character when no higher-priority action applies.
 *   5. Wander randomly when there is nothing else to do.
 *
 * Each helper returns true when it has decided an action for the current tick.
 */
class Bot : public Character {
public:
    /**
     * @brief Creates a bot at the given position and size.
     */
    Bot(const Vector2 position, const Vector2 size) : Character(position, size) {}

    /**
     * @brief Evaluates the bot's behaviour priorities and chooses its action for the current tick.
     *
     * The bot first handles danger and then considers power-ups, walls, enemies, and finally wandering.
     */
    void decideNextMove(World& world);

private:
    /**
     * @brief Attempts to move the bot to safety from nearby bomb explosions.
     *
     * Uses a breadth-first search to find a safe escape route when necessary.
     */
    bool tryFlee(const World& world);

    /**
     * @brief Attempts to move the bot towards a nearby PowerUp.
     *
     * Returns true when a PowerUp is found within the bot's detection range.
     */
    bool tryCollectPowerUp(const World& world);

    /**
     * @brief Attempts to destroy a nearby destructible Wall or move towards one.
     */
    bool tryBreakWalls(World& world);

    /**
     * @brief Attempts to attack the nearest Character within the bot's engagement range.
     */
    bool tryAttack(World& world);

    /**
     * @brief Moves one grid step towards the specified target tile.
     *
     * Prefers the axis with the larger distance and falls back to the other axis when blocked.
     */
    bool stepToward(const World& world, int targetCol, int targetRow);

    /**
     * @brief Moves directly towards an exact world position.
     *
     * Used for the final approach when the bot and a PowerUp occupy the same grid cell.
     */
    void chaseExactPosition(Vector2 targetPosition);

    /**
     * @brief Chooses and follows a random safe neighbouring tile when the bot is idle.
     *
     * The selected direction is retained across ticks until it is no longer safe.
     */
    void wander(const World& world);

    /**
     * @brief Finds the first direction of the shortest path to a safe tile.
     *
     * Returns Direction::None when no reachable safe tile can be found.
     */
    [[nodiscard]] Direction findEscapeDirection(const World& world, int startCol, int startRow) const;

    /**
     * @brief Checks whether a grid tile is inside the arena and contains no Wall or live Bomb.
     */
    static bool isWalkable(const World& world, int col, int row);

    /**
     * @brief Checks whether the bot's current hitbox can safely take one step in a direction.
     *
     * Unlike isWalkable(), this accounts for the bot's actual continuous position and hitbox.
     */
    [[nodiscard]] bool isImmediateStepSafe(const World& world, Direction direction) const;

    /**
     * @brief Checks whether a grid tile is walkable and outside all current bomb blast ranges.
     */
    static bool isSafeToStepInto(const World& world, int col, int row);

    /**
     * @brief Returns a direction that moves the bot back towards the centre of its current tile.
     *
     * Returns Direction::None when the bot is already sufficiently centred.
     */
    [[nodiscard]] Direction recenteringDirection() const;

    /**
     * @brief Continues a detour that was started while approaching a blocked target.
     *
     * Returns true while the detour is still being followed.
     */
    bool continueDetour(const World& world);

    /**
     * @brief Checks a movement step using an explicitly specified starting grid tile.
     *
     * This keeps multi-tick detours consistent even while the bot's continuous position changes.
     */
    [[nodiscard]] bool isImmediateStepSafeFrom(const World& world, Direction direction, int col, int row) const;

    /**
     * @brief Tracks the grid position that the bot is currently unable to reach.
     */
    std::pair<int, int> stuckTarget{0, 0};

    /**
     * @brief Number of consecutive ticks spent unable to progress towards stuckTarget.
     */
    int stuckTicks{0};

    /**
     * @brief Updates the watchdog that detects when the bot is physically unable to move.
     */
    void updateStuckWatchdog();

    /**
     * @brief Bot position recorded during the previous watchdog check.
     */
    Vector2 lastWatchdogPosition{};

    /**
     * @brief Number of consecutive ticks during which the bot has failed to move.
     */
    int watchdogStuckTicks{0};

    /**
     * @brief Direction currently being followed while fleeing from a bomb.
     */
    Direction fleeDirection{Direction::None};

    /**
     * @brief Direction currently being followed while wandering.
     */
    Direction wanderDirection{Direction::None};

    /**
     * @brief Indicates whether the bot is currently following a detour around an obstacle.
     */
    bool detouring{false};

    /**
     * @brief Direction used to move during the current detour.
     */
    Direction detourDirection{Direction::None};

    /**
     * @brief Target column of the current detour.
     */
    int detourTargetCol{0};

    /**
     * @brief Target row of the current detour.
     */
    int detourTargetRow{0};
};

} // namespace bomberman::logic