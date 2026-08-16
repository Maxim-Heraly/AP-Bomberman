// logic/src/entities/Bot.cpp
#include "logic/entities/Bot.hpp"
#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Wall.hpp"
#include "logic/entities/PowerUp.hpp"
#include "logic/utils/Random.hpp"
#include "logic/utils/Grid.hpp"
#include <array>
#include <cmath>
#include <queue>
#include <set>
#include <utility>

namespace bomberman::logic {

namespace {
    constexpr std::array<std::pair<int, int>, 4> kGridOffsets{{{0, -1}, {0, 1}, {-1, 0}, {1, 0}}};
    constexpr std::array<Direction, 4> kGridDirections{
        Direction::Up, Direction::Down, Direction::Left, Direction::Right};

    constexpr float kEpsilonX = kTileWidth * 0.5f;
    constexpr float kEpsilonY = kTileHeight * 0.5f;

    // Mirrors World::generateArena()'s tile layout: a kArenaColumns x
    // kArenaRows grid whose cell centers exactly fill [-1, 1] on both axes.
    std::pair<int, int> worldToGridCoords(const Vector2& position) {
        const int col = static_cast<int>(std::round((position.x + 1.f - kTileWidth * 0.5f) / kTileWidth));
        const int row = static_cast<int>(std::round((position.y + 1.f - kTileHeight * 0.5f) / kTileHeight));
        return {col, row};
    }

    Vector2 gridToWorld(int col, int row) {
        return {-1.f + kTileWidth * 0.5f + static_cast<float>(col) * kTileWidth,
                -1.f + kTileHeight * 0.5f + static_cast<float>(row) * kTileHeight};
    }

    bool isCenteredOnTile(const Vector2& position, int col, int row) {
        const Vector2 anchor = gridToWorld(col, row);
        constexpr float kCenterArrivalTolerance = 0.01f; // Mirrors recenteringDirection()'s tolerance.
        return std::abs(position.x - anchor.x) < kCenterArrivalTolerance &&
               std::abs(position.y - anchor.y) < kCenterArrivalTolerance;
    }

    bool inGridBounds(int col, int row) {
        return col >= 0 && col < kArenaColumns && row >= 0 && row < kArenaRows;
    }

    // True axis-aligned overlap test between two arbitrary (position, size)
    // boxes - mirrors EntityModel::intersects() exactly, so it predicts
    // World::handleCollisions()'s verdict precisely (see wouldOverlap()'s
    // use below).
    bool wouldOverlap(const Vector2& posA, const Vector2& sizeA, const Vector2& posB, const Vector2& sizeB) {
        const bool overlapX = std::abs(posA.x - posB.x) < (sizeA.x + sizeB.x) * 0.5f;
        const bool overlapY = std::abs(posA.y - posB.y) < (sizeA.y + sizeB.y) * 0.5f;
        return overlapX && overlapY;
    }

    // World doesn't expose an O(1) grid lookup, so these scan getEntities()
    // and match by grid cell instead - the arena is tiny, so this is cheap.
    std::shared_ptr<Wall> findWallAt(const World& world, int col, int row) {
        if (!inGridBounds(col, row)) return nullptr;
        for (const auto& entity : world.getEntities()) {
            if (!entity->isAlive()) continue;
            auto wall = std::dynamic_pointer_cast<Wall>(entity);
            if (wall && worldToGridCoords(wall->getPosition()) == std::make_pair(col, row)) {
                return wall;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Bomb> findLiveBombAt(const World& world, int col, int row) {
        if (!inGridBounds(col, row)) return nullptr;
        for (const auto& entity : world.getEntities()) {
            auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
            if (bomb && !bomb->hasExploded() && worldToGridCoords(bomb->getPosition()) == std::make_pair(col, row)) {
                return bomb;
            }
        }
        return nullptr;
    }

    // True if `position` currently sits in the blast line (same row or
    // column, within the bomb's radius) of any live Bomb.
    bool isPositionDangerous(const World& world, const Vector2& position) {
        for (const auto& entity : world.getEntities()) {
            auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
            if (!bomb || bomb->hasExploded()) continue;

            const Vector2 bombPos = bomb->getPosition();
            const bool sameRow = std::abs(position.y - bombPos.y) < kEpsilonY;
            const bool sameCol = std::abs(position.x - bombPos.x) < kEpsilonX;
            if (!sameRow && !sameCol) continue;

            const auto radius = static_cast<float>(bomb->getRadius());
            if (sameRow) {
                if (std::abs(position.x - bombPos.x) <= kTileWidth * radius + kEpsilonX) return true;
            } else {
                if (std::abs(position.y - bombPos.y) <= kTileHeight * radius + kEpsilonY) return true;
            }
        }
        return false;
    }
} // namespace

void Bot::decideNextMove(World& world) {
    // Priority order: survival first, then opportunistic power-up
    // collection, then clearing walls to increase the playfield, then
    // hunting other Characters once there's nothing better to do.
    if (tryFlee(world)) {
        detouring = false;
        return;
    }
    if (detouring) {
        if (continueDetour(world)) return;
    }
    if (tryCollectPowerUp(world)) return;
    if (tryBreakWalls(world)) return;
    if (tryAttack(world)) return;

    wander(world);
}

    bool Bot::continueDetour(World& world) {
    if (isCenteredOnTile(getPosition(), detourTargetCol, detourTargetRow)) {
        detouring = false; // Arrived - forget the old target, re-evaluate fresh above.
        return false;
    }
    if (isSafeToStepInto(world, detourTargetCol, detourTargetRow) &&
        isImmediateStepSafe(world, detourDirection)) {
        setMovementInput(detourDirection);
        return true;
        }
    detouring = false; // Became unsafe mid-crossing - abandon, let the normal chain react.
    return false;
}

bool Bot::isWalkable(const World& world, int col, int row) const {
    if (!inGridBounds(col, row)) return false;
    return findWallAt(world, col, row) == nullptr && findLiveBombAt(world, col, row) == nullptr;
}

// Movement here is continuous, not grid-snapped (see Character::update()):
// a Bot that spends a few ticks moving along one axis and then turns onto
// the other keeps whatever fractional offset it had on the axis it just
// left - Character::update() only ever touches one axis per tick, so nothing
// makes that leftover offset re-center on its own. isWalkable()/findWallAt()
// only ask "is a Wall/Bomb's own (always tile-centered) position rounded to
// this same grid cell?" - that's the right question for a HYPOTHETICAL,
// perfectly-centered Bot, but it says nothing about whether THIS Bot's real,
// slightly-off-center hitbox would clip a diagonally-adjacent obstacle while
// crossing into that cell. That gap is exactly what leaves a fleeing Bot
// standing still (silently reverted by World::handleCollisions every tick,
// even while it keeps re-issuing the same movement input) right next to its
// own still-ticking Bomb. Predicting the real collision - not just the
// idealized one - requires testing the Bot's ACTUAL current position (not a
// rounded/anchored stand-in) against every Wall/Bomb with the same
// axis-aligned overlap test World::handleCollisions() itself uses.
bool Bot::isImmediateStepSafe(const World& world, Direction direction) const {
    // The axis this Bot is actually moving along will, once this step
    // completes, sit exactly at the target tile's canonical (tile-center)
    // value; the OTHER axis never changes during a single-direction move
    // (Character::update() only ever touches one axis per tick), so it
    // stays at whatever value this Bot's real, continuous position
    // currently has - drifted or not. Combining "canonical on the moving
    // axis, real/raw on the static axis" gives the actual worst-case
    // overlap point of this transit - the same point
    // World::handleCollisions() would eventually test against. Using the
    // raw position on BOTH axes (i.e. just offsetting the current,
    // possibly-drifted position by a full tile step) instead compounds
    // whatever drift already exists into the moving axis too, which can
    // just as easily manufacture a false "blocked" reading against some
    // unrelated wall as it can miss a real one.
    const auto [col, row] = worldToGridCoords(getPosition());
    const Vector2 step = directionToVector(direction);
    const Vector2 canonicalTarget = gridToWorld(col + static_cast<int>(step.x), row + static_cast<int>(step.y));
    const Vector2 candidate = (step.x != 0.f)
        ? Vector2{canonicalTarget.x, getPosition().y}
        : Vector2{getPosition().x, canonicalTarget.y};

    for (const auto& entity : world.getEntities()) {
        if (!entity->isAlive()) continue;
        if (auto wall = std::dynamic_pointer_cast<Wall>(entity)) {
            if (wouldOverlap(candidate, getSize(), wall->getPosition(), wall->getSize())) return false;
        } else if (auto bomb = std::dynamic_pointer_cast<Bomb>(entity)) {
            if (!bomb->hasExploded() && wouldOverlap(candidate, getSize(), bomb->getPosition(), bomb->getSize())) return false;
        }
    }
    return true;
}

bool Bot::isSafeToStepInto(const World& world, int col, int row) const {
    return isWalkable(world, col, row) && !isPositionDangerous(world, gridToWorld(col, row));
}

// A Bot can end up genuinely boxed in - not by real Walls/Bombs on every
// side, but because isImmediateStepSafe() (correctly) refuses a direction
// whose transit its own residual drift would clip. That drift only ever
// comes from a direction change made before this Bot finished crossing the
// tile it was previously in (see isImmediateStepSafe()'s comment) - which
// means nudging it back towards its CURRENT tile's own center is always a
// safe, useful thing to try: it either measurably reduces the drift that's
// causing the lockout, or - if something is already flush against it in
// that direction - World::handleCollisions() simply reverts the tiny nudge,
// leaving the Bot no worse off than standing still. Because the move never
// leaves the current tile, it needs no separate walkability check.
Direction Bot::recenteringDirection() const {
    const Vector2 pos = getPosition();
    const auto [col, row] = worldToGridCoords(pos);
    const Vector2 anchor = gridToWorld(col, row);

    constexpr float kRecenterTolerance = 0.01f; // World units - well under any collision margin.
    const float dx = anchor.x - pos.x;
    const float dy = anchor.y - pos.y;

    // Correct whichever axis is further off-center first.
    if (std::abs(dx) >= std::abs(dy)) {
        if (std::abs(dx) > kRecenterTolerance) return dx > 0.f ? Direction::Right : Direction::Left;
    } else {
        if (std::abs(dy) > kRecenterTolerance) return dy > 0.f ? Direction::Down : Direction::Up;
    }
    return Direction::None; // Already centered - nothing to correct.
}

bool Bot::tryFlee(World& world) {
    if (!isPositionDangerous(world, getPosition())) {
        fleeDirection = Direction::None; // Not fleeing (anymore) - a future flee should plan fresh.
        return false;
    }

    const auto [col, row] = worldToGridCoords(getPosition());

    // If already committed to an escape direction from an earlier tick, and
    // it's still walkable AND the Bot's real (possibly drifted) hitbox can
    // actually step that way right now, keep going that way rather than
    // recomputing a fresh BFS plan every single frame. Re-checking
    // isImmediateStepSafe() every tick (not just isWalkable()) matters
    // because a direction that looked clear when first chosen can still be
    // physically blocked in practice by drift-induced clipping (see
    // isImmediateStepSafe()'s comment) - without this, a Bot can keep
    // "committing" to a direction it never actually moves in, and sit still
    // until its own Bomb goes off.
    if (fleeDirection != Direction::None) {
        const Vector2 offset = directionToVector(fleeDirection);
        const int nextCol = col + static_cast<int>(offset.x);
        const int nextRow = row + static_cast<int>(offset.y);
        if (isWalkable(world, nextCol, nextRow) && isImmediateStepSafe(world, fleeDirection)) {
            setMovementInput(fleeDirection);
            return true;
        }
    }

    fleeDirection = findEscapeDirection(world, col, row);
    if (fleeDirection == Direction::None) {
        // No reachable tile currently registers as safe - possibly because
        // this Bot's own drift makes an otherwise-clear corridor exit look
        // blocked (see isImmediateStepSafe()). Nudging back towards this
        // tile's center can't make things worse, and often clears the drift
        // that was the actual problem within a tick or two.
        fleeDirection = recenteringDirection();
    }
    setMovementInput(fleeDirection);
    return true; // Whether or not a safe cell was actually found, fleeing has "claimed" this tick.
}

Direction Bot::findEscapeDirection(const World& world, int startCol, int startRow) const {
    struct Node {
        int col;
        int row;
        int firstStepIndex; // Which of the 4 initial directions this node's path branched from.
    };

    std::queue<Node> frontier;
    std::set<std::pair<int, int>> visited{{startCol, startRow}};

    // Seeding the frontier is the ONE place a real (not just idealized)
    // step needs verifying: everything the BFS explores beyond this first
    // hop is a hypothetical, perfectly-centered future cell, but this very
    // first step is the one the Bot's real, possibly drifted hitbox has to
    // take right now - see isImmediateStepSafe().
    for (int i = 0; i < 4; ++i) {
        const int c = startCol + kGridOffsets[i].first;
        const int r = startRow + kGridOffsets[i].second;
        if (isWalkable(world, c, r) && isImmediateStepSafe(world, kGridDirections[i]) && visited.insert({c, r}).second) {
            frontier.push({c, r, i});
        }
    }

    // The grid only has kArenaColumns*kArenaRows cells in total, so a full
    // BFS is trivially cheap - no need for an artificial search-depth
    // cutoff.
    while (!frontier.empty()) {
        const Node node = frontier.front();
        frontier.pop();

        if (!isPositionDangerous(world, gridToWorld(node.col, node.row))) {
            return kGridDirections[node.firstStepIndex]; // Found the nearest safe tile - go that way.
        }

        for (int i = 0; i < 4; ++i) {
            const int c = node.col + kGridOffsets[i].first;
            const int r = node.row + kGridOffsets[i].second;
            if (isWalkable(world, c, r) && visited.insert({c, r}).second) {
                frontier.push({c, r, node.firstStepIndex}); // Keep the ORIGINAL first step, not this one.
            }
        }
    }

    return Direction::None; // Genuinely boxed in on every side - nothing left to do but hope.
}

void Bot::wander(World& world) {
    // Nothing urgent to do - wander into a random open neighbour so the Bot
    // keeps exploring the arena instead of standing completely still.
    // Commits to wanderDirection across ticks instead of re-rolling a fresh
    // random direction every single frame (which would otherwise make an
    // idle Bot visibly jitter in place), but - like tryFlee() - re-checks
    // isImmediateStepSafe() every tick rather than trusting a stale choice,
    // since drift can turn a previously-clear direction into a real one.
    const auto [col, row] = worldToGridCoords(getPosition());

    if (wanderDirection != Direction::None) {
        const Vector2 offset = directionToVector(wanderDirection);
        const int nextCol = col + static_cast<int>(offset.x);
        const int nextRow = row + static_cast<int>(offset.y);
        if (isSafeToStepInto(world, nextCol, nextRow) && isImmediateStepSafe(world, wanderDirection)) {
            setMovementInput(wanderDirection);
            return;
        }
    }

    // Randomly order the 4 directions (Fisher-Yates) and try each exactly
    // once. Sampling with replacement here would mean: if only one of the
    // four is actually viable - common in this checkerboard maze, where a
    // tile often has just one or two open neighbours - each independent
    // draw only has a 1-in-4 chance of finding it, so roughly a third of
    // the time all 4 draws miss it even though a valid move existed. That
    // silent, purely-random "give up" is exactly what shows up as visible
    // jitter/stutter - trying each direction once instead guarantees any
    // viable one gets found.
    std::array<Direction, 4> shuffled = kGridDirections;
    for (int i = 3; i > 0; --i) {
        const int j = Random::getInstance().getInt(0, i);
        std::swap(shuffled[i], shuffled[j]);
    }
    for (const Direction candidate : shuffled) {
        const Vector2 offset = directionToVector(candidate);
        if (isSafeToStepInto(world, col + static_cast<int>(offset.x), row + static_cast<int>(offset.y)) &&
            isImmediateStepSafe(world, candidate)) {
            wanderDirection = candidate;
            setMovementInput(candidate);
            return;
        }
    }
    wanderDirection = Direction::None;
    setMovementInput(recenteringDirection()); // Nothing else viable - see recenteringDirection()'s comment.
}

bool Bot::tryCollectPowerUp(World& world) {
    constexpr int kDetectionRange = 5; // Tiles - a Bot only "notices" powerups this close.
    const auto [col, row] = worldToGridCoords(getPosition());

    int bestDistance = kDetectionRange + 1;
    int targetCol = 0;
    int targetRow = 0;
    Vector2 targetPosition{};
    bool found = false;

    for (const auto& entity : world.getEntities()) {
        auto powerUp = std::dynamic_pointer_cast<PowerUp>(entity);
        if (!powerUp || !powerUp->isAlive()) continue;

        const auto [pCol, pRow] = worldToGridCoords(powerUp->getPosition());
        const int distance = std::abs(pCol - col) + std::abs(pRow - row);
        if (distance < bestDistance) {
            bestDistance = distance;
            targetCol = pCol;
            targetRow = pRow;
            targetPosition = powerUp->getPosition();
            found = true;
        }
    }

    if (!found) return false;

    if (targetCol == col && targetRow == row) {
        // Already in the powerup's grid cell, but World::handleCollisions()
        // only actually grants the pickup once this Bot's hitbox truly
        // overlaps the powerup's exact (continuous) position - "same grid
        // cell" isn't precise enough. Do a precise final approach instead
        // for this last short stretch (the whole cell is guaranteed open -
        // a powerup can't spawn inside a wall - so there's nothing to route
        // around here).
        chaseExactPosition(targetPosition);
        return true;
    }
    return stepToward(world, targetCol, targetRow);
}

bool Bot::tryBreakWalls(World& world) {
    const auto [col, row] = worldToGridCoords(getPosition());

    // Highest priority within this behaviour: a destructible wall directly
    // next to us - blow it up right away rather than walking towards it.
    // Bombing itself needs a free bomb slot, but approaching a wall (below)
    // doesn't - a Bot that already has a Bomb out should still close the
    // distance to the next one, so it can act the instant that Bomb clears
    // instead of standing idle for the whole ~3s fuse (see tryAttack(),
    // which never gated its own stepToward() on canPlaceBomb() either).
    if (canPlaceBomb()) {
        for (const auto& [dCol, dRow] : kGridOffsets) {
            if (auto wall = findWallAt(world, col + dCol, row + dRow)) {
                if (wall->isDestructible()) {
                    world.placeBomb(*this);
                    return true;
                }
            }
        }
    }

    // Otherwise, head towards the closest destructible wall within a
    // modest search radius, so the Bot actively increases its playfield
    // over time instead of only reacting to what's adjacent.
    constexpr int kSearchRadius = 6;
    int bestDistance = kSearchRadius + 1;
    int targetCol = 0;
    int targetRow = 0;
    bool found = false;

    for (const auto& entity : world.getEntities()) {
        auto wall = std::dynamic_pointer_cast<Wall>(entity);
        if (!wall || !wall->isAlive() || !wall->isDestructible()) continue;

        const auto [wCol, wRow] = worldToGridCoords(wall->getPosition());
        const int distance = std::abs(wCol - col) + std::abs(wRow - row);
        if (distance < bestDistance) {
            bestDistance = distance;
            targetCol = wCol;
            targetRow = wRow;
            found = true;
        }
    }

    if (!found) return false; // No destructible walls left nearby.
    return stepToward(world, targetCol, targetRow);
}

bool Bot::tryAttack(World& world) {
    constexpr int kEngageRange = 4; // Tiles - how far away an enemy has to be noticed and chased.
    const auto [col, row] = worldToGridCoords(getPosition());

    int bestDistance = kEngageRange + 1;
    int targetCol = 0;
    int targetRow = 0;
    bool found = false;

    for (const auto& entity : world.getEntities()) {
        auto character = std::dynamic_pointer_cast<Character>(entity);
        if (!character || character.get() == this || !character->isAlive()) continue;

        const auto [oCol, oRow] = worldToGridCoords(character->getPosition());
        const int distance = std::abs(oCol - col) + std::abs(oRow - row);
        if (distance < bestDistance) {
            bestDistance = distance;
            targetCol = oCol;
            targetRow = oRow;
            found = true;
        }
    }

    if (!found) return false;
    if (bestDistance <= 1 && canPlaceBomb()) {
        world.placeBomb(*this);
        return true;
    }
    return stepToward(world, targetCol, targetRow);
}

bool Bot::stepToward(World& world, int targetCol, int targetRow) {
    const auto [col, row] = worldToGridCoords(getPosition());
    const int dCol = targetCol - col;
    const int dRow = targetRow - row;

    if (dCol == 0 && dRow == 0) {
        setMovementInput(Direction::None);
        stuckTicks = 0;
        return false; // Already there.
    }

    const Direction horizontal = dCol > 0 ? Direction::Right : Direction::Left;
    const Direction vertical = dRow > 0 ? Direction::Down : Direction::Up;
    const bool preferHorizontal = std::abs(dCol) >= std::abs(dRow);

    // Try the axis with the bigger gap first; if it's blocked (or would
    // walk through an active blast, or would clip an obstacle given this
    // Bot's real current drift - see isImmediateStepSafe()), fall back to
    // the other axis rather than getting stuck waiting on one path.
    if (preferHorizontal && dCol != 0 && isSafeToStepInto(world, col + (dCol > 0 ? 1 : -1), row) &&
        isImmediateStepSafe(world, horizontal)) {
        setMovementInput(horizontal);
        stuckTicks = 0;
        return true;
    }
    if (dRow != 0 && isSafeToStepInto(world, col, row + (dRow > 0 ? 1 : -1)) &&
        isImmediateStepSafe(world, vertical)) {
        setMovementInput(vertical);
        stuckTicks = 0;
        return true;
    }
    if (dCol != 0 && isSafeToStepInto(world, col + (dCol > 0 ? 1 : -1), row) &&
        isImmediateStepSafe(world, horizontal)) {
        setMovementInput(horizontal);
        stuckTicks = 0;
        return true;
    }

        if (dCol != 0 && isSafeToStepInto(world, col + (dCol > 0 ? 1 : -1), row) &&
        isImmediateStepSafe(world, horizontal)) {
        setMovementInput(horizontal);
        stuckTicks = 0;
        return true;
    }

    // Straight-line case: the target shares this Bot's row or column
    // exactly, so neither axis fallback above had another axis to try.
    // If it's specifically an indestructible pillar blocking the one
    // direction available, don't try to cleverly keep tracking THIS
    // target while sidestepping around it - hand off to a short, blind,
    // target-agnostic detour instead (see decideNextMove()/
    // continueDetour()). Pillars only ever occupy cells where BOTH grid
    // coordinates are even, so the perpendicular neighbour tile is always
    // guaranteed pillar-free.
    if (dCol == 0 || dRow == 0) {
        const Direction blockedDirection = (dRow != 0) ? vertical : horizontal;
        const Vector2 blockedOffset = directionToVector(blockedDirection);
        const auto obstacle = findWallAt(world, col + static_cast<int>(blockedOffset.x),
                                                  row + static_cast<int>(blockedOffset.y));

        if (obstacle && !obstacle->isDestructible()) {
            const std::array<Direction, 2> perpendicular = (dCol == 0)
                ? std::array<Direction, 2>{Direction::Left, Direction::Right}
            : std::array<Direction, 2>{Direction::Up, Direction::Down};

            for (Direction side : perpendicular) {
                const Vector2 offset = directionToVector(side);
                const int sCol = col + static_cast<int>(offset.x);
                const int sRow = row + static_cast<int>(offset.y);
                if (isSafeToStepInto(world, sCol, sRow) && isImmediateStepSafe(world, side)) {
                    detouring = true;
                    detourDirection = side;
                    detourTargetCol = sCol;
                    detourTargetRow = sRow;
                    setMovementInput(side);
                    stuckTicks = 0;
                    return true;
                }
            }
        }
    }

    // Genuinely can't progress towards (targetCol, targetRow) right now.
    // Returning false here would hand this tick to a lower-priority
    // behaviour - wander() in particular - which has no idea this Bot is
    // deliberately camped at the edge of an obstacle it's working towards
    // (e.g. a wall it can't bomb yet because it already has one out): it
    // would happily walk the Bot right back out the way it came, and next
    // tick this function beelines straight back in, forever. Holding
    // position instead (via recenteringDirection(), or None if already
    // centered) avoids that tug-of-war - but only for a bounded number of
    // ticks: if the SAME target is still unreachable after kStuckTimeout
    // ticks, this is more likely a genuinely unreachable target (e.g.
    // behind an indestructible pillar with no path around) than a
    // temporary wait, so give up and let a lower-priority behaviour
    // explore instead of holding forever.
    const auto currentTarget = std::make_pair(targetCol, targetRow);
    if (stuckTarget != currentTarget) {
        stuckTarget = currentTarget;
        stuckTicks = 0;
    }
    ++stuckTicks;

    constexpr int kStuckTimeout = 90; // ~1.5s at 60 ticks/s.
    if (stuckTicks > kStuckTimeout) {
        return false;
    }

    setMovementInput(recenteringDirection());
    return true;
}

void Bot::chaseExactPosition(Vector2 targetPosition) {
    const Vector2 myPosition = getPosition();
    const float dx = targetPosition.x - myPosition.x;
    const float dy = targetPosition.y - myPosition.y;

    // Small tolerance so the Bot doesn't jitter back and forth trying to
    // land on the exact floating-point target position - collision
    // (World::handleCollisions()) takes over as soon as the hitboxes
    // overlap, well before this tolerance would ever matter.
    constexpr float kArrivalEpsilon = 0.01f;
    if (std::abs(dx) < kArrivalEpsilon && std::abs(dy) < kArrivalEpsilon) {
        setMovementInput(Direction::None);
        return;
    }

    if (std::abs(dx) >= std::abs(dy)) {
        setMovementInput(dx > 0.f ? Direction::Right : Direction::Left);
    } else {
        setMovementInput(dy > 0.f ? Direction::Down : Direction::Up);
    }
}

} // namespace bomberman::logic