#pragma once

#include "logic/entities/Character.hpp"
#include <utility>

namespace bomberman::logic {

class World;

/**
 * @brief Computer-controlled Character behaviour: a priority list
 * re-evaluated every tick by decideNextMove().
 *
 *   1. Flee any Bomb whose blast would currently reach this Bot. A
 *      breadth-first search over the arena grid finds the *nearest*
 *      safe tile, even if that takes several steps through the arena's
 *      1-wide corridors.
 *   2. Chase down any nearby PowerUp.
 *   3. Otherwise, hunt down and bomb the nearest destructible Wall.
 *   4. Once no destructible Walls are left (or nothing else applies),
 *      fall back to attacking the nearest Character.
 *   5. If nothing above applies, wander into a random open neighbour
 *      tile instead of standing still.
 *
 * Each helper below returns true once it has decided on an action for
 * this tick, so decideNextMove() just chains them in priority order.
 */
class Bot : public Character {
public:
    Bot(const Vector2 position, const Vector2 size) : Character(position, size) {}

    /// Called once per tick (by World::update(), before entities are
    /// updated) - inspects the World and calls setMovementInput() /
    /// World::placeBomb() accordingly.
    void decideNextMove(World& world);

private:
    bool tryFlee(const World& world);
    bool tryCollectPowerUp(const World& world);
    bool tryBreakWalls(World& world);
    bool tryAttack(World& world);

    /// Points this Bot one grid step towards (targetCol, targetRow),
    /// preferring whichever axis currently has the bigger gap; falls
    /// back to the other axis if that direction turns out to be
    /// blocked or dangerous. See the .cpp for what happens when
    /// neither axis works.
    bool stepToward(const World& world, int targetCol, int targetRow);

    /// Fine-grained final approach towards an exact world position -
    /// used once this Bot is already standing in a PowerUp's own grid
    /// cell, since World::handleCollisions() only actually grants the
    /// pickup once the (continuous) hitboxes truly overlap.
    void chaseExactPosition(Vector2 targetPosition);

    /// Wander into a random open neighbour when nothing higher-priority
    /// needs doing. Commits to the chosen direction (via
    /// wanderDirection) across ticks instead of re-rolling a fresh
    /// random direction every single frame, which would otherwise make
    /// idle Bots jitter in place rather than actually wandering.
    void wander(const World& world);

    /// Breadth-first search over walkable grid tiles, starting from
    /// (startCol, startRow), for the nearest tile where this Bot would
    /// not be caught in any live Bomb's blast. Returns the direction of
    /// the first step of that shortest path, or Direction::None if no
    /// safe tile is reachable at all.
    [[nodiscard]] Direction findEscapeDirection(const World& world, int startCol, int startRow) const;

    /// True if (col, row) is inside the arena and has no Wall or live
    /// Bomb standing on it.
    static bool isWalkable(const World& world, int col, int row);

    /// Like isWalkable(), but tests this Bot's REAL current (continuous,
    /// possibly off-tile-center) hitbox against every Wall/Bomb using the
    /// same axis-aligned overlap test World::handleCollisions() uses -
    /// rather than comparing rounded grid cells, which silently assumes
    /// a perfectly-centered Bot. See the .cpp for why that assumption
    /// can be wrong and leave a Bot stuck standing on its own Bomb.
    [[nodiscard]] bool isImmediateStepSafe(const World& world, Direction direction) const;

    /// Like isWalkable(), but also refuses any currently-dangerous
    /// tile. Used everywhere except findEscapeDirection()'s BFS, which
    /// needs to be able to path *through* a dangerous tile to reach a
    /// safe one on the other side of it.
    static bool isSafeToStepInto(const World& world, int col, int row);

    /// Fallback used whenever every direction otherwise looks blocked:
    /// returns the direction that nudges this Bot back towards the
    /// center of whichever tile it currently (per worldToGridCoords)
    /// occupies, or Direction::None if it's already centered within a
    /// small tolerance. See the .cpp for why a Bot can end up needing
    /// this - and why it's always safe to try even without an explicit
    /// walkability check.
    [[nodiscard]] Direction recenteringDirection() const;

    /// The direction chosen by the most recent tryFlee() call - a Bot
    /// commits to an escape route instead of recomputing (and
    /// potentially reversing) a brand-new BFS plan every single tick.
    Direction fleeDirection{Direction::None};

    /// The direction chosen by the most recent wander() call.
    Direction wanderDirection{Direction::None};

    bool detouring{false};
    Direction detourDirection{Direction::None};
    int detourTargetCol{0};
    int detourTargetRow{0};

    /// Finishes an in-progress detour started by stepToward(): keeps
    /// stepping in detourDirection until this Bot is actually
    /// centered on (detourTargetCol, detourTargetRow) - not just
    /// until worldToGridCoords() rounds over to it - re-verifying
    /// safety every tick. Returns true while still mid-crossing
    /// (claims this tick's move); returns false once the detour is
    /// complete or had to be abandoned, so decideNextMove() falls
    /// through to a fresh, target-agnostic re-evaluation.
    bool continueDetour(const World& world);

    /// Like isImmediateStepSafe(), but tests the step from an explicit
    /// (col, row) departure tile instead of deriving it by rounding
    /// getPosition(). Needed by continueDetour(), which must keep validating
    /// the *same* crossing over several ticks - re-deriving the departure tile
    /// from getPosition() each tick starts reporting the arrival tile as soon
    /// as this Bot passes the tile's midpoint (well before isCenteredOnTile()
    /// considers it arrived), which tests one tile too far and freezes the Bot
    /// straddling the crossover.
    [[nodiscard]] bool isImmediateStepSafeFrom(const World& world, Direction direction, int col, int row) const;

    /// Which (col, row) stepToward() last failed to make progress
    /// towards, and for how many consecutive ticks it's been stuck on
    /// that same target - see stepToward()'s .cpp comment for why this
    /// exists (it bounds how long a Bot holds position at a blocked
    /// target before giving up and letting a lower-priority behaviour,
    /// e.g. wander(), try something else).
    std::pair<int, int> stuckTarget{0, 0};
    int stuckTicks{0};

    void updateStuckWatchdog();

    Vector2 lastWatchdogPosition{};
    int watchdogStuckTicks{0};
};

} // namespace bomberman::logic