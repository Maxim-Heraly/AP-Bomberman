// logic/include/logic/entities/Bot.hpp
#pragma once

#include "logic/entities/Character.hpp"
#include <array>

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
     *
     * ROOT CAUSE OF THE "FREEZE" BUG (fixed by findPathDirection()):
     * stepToward() used to be a purely greedy heuristic - "move along
     * whichever axis currently has the bigger delta, falling back to the
     * other axis if blocked" - with NO ability to detour. In this arena's
     * checkerboard rows, every tile has an indestructible Wall immediately
     * to its left AND right by construction. Whenever a target (a wall to
     * bomb, an enemy to chase) sits in the exact same row as the Bot
     * (dRow == 0) behind one of those pillars, the greedy code refused to
     * even attempt a vertical detour - the guard is `if (dRow != 0 && ...)`,
     * and dRow really is 0 - so it retried the same blocked horizontal
     * direction twice and then fell back to recenteringDirection(), which
     * returns Direction::None once already tile-centered. The Bot's
     * movement input became None forever: a genuine freeze, not a
     * false-positive later reverted by World::handleCollisions(), so the
     * directionCooldown/isSuspectDirection machinery (which only fires
     * on a *reverted* attempted movement) never caught it. This is also
     * exactly why a Bot only ever escaped once a HIGHER-priority behaviour
     * (tryCollectPowerUp, priority 2) started calling stepToward() with a
     * different target that usually wasn't row/column-locked behind a
     * pillar - proximity of another Character was incidental, not causal.
     * findPathDirection() replaces the greedy heuristic with a real BFS
     * (mirroring findEscapeDirection()'s existing approach), so it always
     * finds a route - including detours - whenever one exists.
     *
     * Stuck detection: isImmediateStepSafe() is still only a one-shot
     * prediction of the destination tile, not a full simulation of the
     * transit, so it can still occasionally approve a step that
     * World::handleCollisions() reverts once movement is integrated
     * tick-by-tick. directionCooldown[]/isSuspectDirection() remain in
     * place as a safeguard against THAT (separate, narrower) class of
     * disagreement, independent of the pathfinding fix above.
     */
    class Bot : public Character {
    public:
        Bot(Vector2 position, Vector2 size) : Character(position, size) {}

        /// Called once per tick (by World::update(), before entities are
        /// updated) - inspects the World and calls setMovementInput() /
        /// World::placeBomb() accordingly.
        void decideNextMove(World& world);

    private:
        bool tryFlee(World& world);
        bool tryCollectPowerUp(World& world);
        bool tryBreakWalls(World& world);
        bool tryAttack(World& world);

        /// Moves this Bot one grid step towards (targetCol, targetRow),
        /// using findPathDirection() for real shortest-path routing
        /// (including detours around static obstacles - see the class
        /// comment for why a naive greedy approach isn't enough). Returns
        /// false if already at (or, for an unwalkable target like a Wall,
        /// already adjacent to) the target; true otherwise.
        bool stepToward(World& world, int targetCol, int targetRow);

        /// Breadth-first search over walkable grid tiles, starting from
        /// (startCol, startRow), for the shortest route to (targetCol,
        /// targetRow) - or, if that tile isn't walkable (e.g. a Wall
        /// occupies it), to any tile directly adjacent to it. Returns the
        /// direction of the first step of that route, or Direction::None
        /// if no route is currently available (already there/adjacent,
        /// every immediate neighbour is on cooldown, or the target is
        /// genuinely unreachable).
        [[nodiscard]] Direction findPathDirection(const World& world, int startCol, int startRow,
                                                   int targetCol, int targetRow) const;

        /// True if (col, row) is either exactly the target (when the
        /// target tile is walkable) or exactly adjacent to it (when it
        /// isn't, e.g. a Wall). Shared by stepToward()'s early-out and
        /// findPathDirection()'s BFS goal test so both agree on what
        /// "arrived" means.
        [[nodiscard]] bool hasReachedTarget(const World& world, int col, int row,
                                            int targetCol, int targetRow) const;

        /// Fine-grained final approach towards an exact world position -
        /// used once this Bot is already standing in a PowerUp's own grid
        /// cell, since World::handleCollisions() only actually grants the
        /// pickup once the (continuous) hitboxes truly overlap.
        void chaseExactPosition(Vector2 targetPosition);

        /// Wander into a random open neighbour when nothing higher-priority
        /// needs doing. Commits to the chosen direction (via
        /// wanderDirection) across ticks instead of re-rolling a fresh
        /// random direction every single frame, which would otherwise make
        /// an idle Bot visibly jitter in place, but - like tryFlee() -
        /// re-checks isImmediateStepSafe() every tick rather than trusting
        /// a stale choice, since drift can turn a previously-clear
        /// direction into a real one. Tries every direction (in a
        /// shuffled, non-repeating order) rather than sampling with
        /// replacement, so a single-open-neighbour corridor is found
        /// reliably instead of ~32% of the time being missed.
        void wander(World& world);

        /// Breadth-first search over walkable grid tiles, starting from
        /// (startCol, startRow), for the nearest tile where this Bot would
        /// not be caught in any live Bomb's blast. Returns the direction of
        /// the first step of that shortest path, or Direction::None if no
        /// safe tile is reachable at all.
        [[nodiscard]] Direction findEscapeDirection(const World& world, int startCol, int startRow) const;

        /// True if (col, row) is inside the arena and has no Wall or live
        /// Bomb standing on it.
        [[nodiscard]] bool isWalkable(const World& world, int col, int row) const;

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
        [[nodiscard]] bool isSafeToStepInto(const World& world, int col, int row) const;

        /// Fallback used whenever every direction otherwise looks blocked:
        /// returns the direction that nudges this Bot back towards the
        /// center of whichever tile it currently (per worldToGridCoords)
        /// occupies, or Direction::None if it's already centered within a
        /// tolerance derived from this tick's actual step size. Correcting
        /// by a fixed fractional step (rather than a full tile step) is
        /// what keeps this convergent - a full-tile correction can overshoot
        /// past center and flip the drift to the opposite sign, which then
        /// gets "corrected" right back next tick, producing a stable,
        /// endless 2-tile ping-pong. See the .cpp for details.
        [[nodiscard]] Direction recenteringDirection() const;

        /// Re-evaluates directionCooldown[] by comparing this Bot's current
        /// position against its position from one tick ago (see the .cpp
        /// for why that comparison is the right one given World::update()'s
        /// call order), and ages down every existing cooldown by one tick.
        /// Called once at the very start of decideNextMove().
        void updateStuckTracking();

        /// Single choke point for actually issuing movement: records
        /// `direction` as lastAttemptedDirection (so the next tick's
        /// updateStuckTracking() knows which direction to penalize if it
        /// turns out not to have produced real movement) and forwards to
        /// Character::setMovementInput(). Every place in this class that
        /// would otherwise call setMovementInput() directly calls this
        /// instead.
        void commitMovement(Direction direction);

        /// Maps a cardinal Direction to its slot in directionCooldown[].
        /// Direction::None is not a valid input (callers already guard
        /// against it) and maps to slot 0 defensively.
        [[nodiscard]] static std::size_t cooldownIndex(Direction direction);

        /// True while `direction`'s cooldown (see directionCooldown) is
        /// still active - i.e. it has recently been requested and provably
        /// failed to produce any actual position change. Direction-choosing
        /// code treats a direction on cooldown as unusable even if the
        /// predictive helpers (isWalkable/isSafeToStepInto/
        /// isImmediateStepSafe) still approve it, since those helpers are
        /// exactly what got fooled last time.
        [[nodiscard]] bool isSuspectDirection(Direction direction) const;

        /// The direction chosen by the most recent tryFlee() call - a Bot
        /// commits to an escape route instead of recomputing (and
        /// potentially reversing) a brand-new BFS plan every single tick.
        Direction fleeDirection{Direction::None};

        /// The direction chosen by the most recent wander() call.
        Direction wanderDirection{Direction::None};

        /// The direction most recently issued via commitMovement(). Tracked
        /// separately from fleeDirection/wanderDirection so it also covers
        /// directions chosen by stepToward(), chaseExactPosition(), and
        /// recenteringDirection() - i.e. every source of movement this
        /// class has, not just the two cached "plans".
        Direction lastAttemptedDirection{Direction::None};

        /// Per-direction (Up/Down/Left/Right) cooldown counters. A positive
        /// entry means that direction is currently suspect (see
        /// isSuspectDirection()). One slot per direction lets two different
        /// directions be penalized at once, which matters at a fork with
        /// exactly two viable paths - see isSuspectDirection().
        std::array<int, 4> directionCooldown{0, 0, 0, 0};

        /// How many ticks a direction stays on cooldown after being caught
        /// producing no real movement. Kept short so a direction that
        /// becomes genuinely usable again (e.g. an obstacle moved away)
        /// isn't refused for long, while still comfortably outlasting a
        /// single unlucky tick.
        static constexpr int kCooldownTicks = 4;
    };

} // namespace bomberman::logic