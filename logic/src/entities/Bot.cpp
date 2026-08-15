// logic/src/entities/Bot.cpp
#include "logic/entities/Bot.hpp"
#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Wall.hpp"
#include "logic/entities/PowerUp.hpp"
#include "logic/utils/Random.hpp"
#include "logic/utils/Grid.hpp"
#include "logic/utils/Stopwatch.hpp"
#include <algorithm>
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

std::size_t Bot::cooldownIndex(Direction direction) {
    switch (direction) {
        case Direction::Up:    return 0;
        case Direction::Down:  return 1;
        case Direction::Left:  return 2;
        case Direction::Right: return 3;
        case Direction::None:  break;
    }
    return 0; // Defensive default - callers guard against Direction::None already.
}

void Bot::updateStuckTracking() {
    for (int& cooldown : directionCooldown) {
        if (cooldown > 0) --cooldown;
    }

    // World::update() calls decideNextMove() for every Bot BEFORE calling
    // Character::update(deltaTime) for anyone, and Character::update()
    // sets previousPosition = position right before (maybe) moving. So at
    // the moment THIS function runs (start of decideNextMove(), tick T),
    // comparing getPosition() to getPreviousPosition() tells us whether
    // tick T-1's committed direction actually changed this Bot's resolved
    // position, or was reverted by World::handleCollisions() (or was
    // Direction::None to begin with, which is excluded below).
    const bool madeNoProgress = getPosition().x == getPreviousPosition().x &&
                                 getPosition().y == getPreviousPosition().y;

    if (lastAttemptedDirection != Direction::None && madeNoProgress) {
        directionCooldown[cooldownIndex(lastAttemptedDirection)] = kCooldownTicks;
    }
}

void Bot::commitMovement(Direction direction) {
    lastAttemptedDirection = direction;
    setMovementInput(direction);
}

bool Bot::isSuspectDirection(Direction direction) const {
    return direction != Direction::None && directionCooldown[cooldownIndex(direction)] > 0;
}

void Bot::decideNextMove(World& world) {
    updateStuckTracking();

    // Priority order: survival first, then opportunistic power-up
    // collection, then clearing walls to increase the playfield, then
    // hunting other Characters once there's nothing better to do.
    if (tryFlee(world)) return;
    if (tryCollectPowerUp(world)) return;
    if (tryBreakWalls(world)) return;
    if (tryAttack(world)) return;

    wander(world);
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
// crossing into that cell. Predicting the real collision - not just the
// idealized one - requires testing the Bot's ACTUAL current position (not a
// rounded/anchored stand-in) against every Wall/Bomb with the same
// axis-aligned overlap test World::handleCollisions() itself uses.
//
// Note this is still only a ONE-SHOT prediction of the destination tile,
// not a simulation of every intermediate frame of the crossing - it can
// still occasionally disagree with what World::handleCollisions() decides
// once movement is actually integrated tick-by-tick. isSuspectDirection()
// exists specifically to catch and correct for that residual disagreement
// (a narrower issue than the pathfinding bug described in Bot.hpp).
bool Bot::isImmediateStepSafe(const World& world, Direction direction) const {
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

Direction Bot::recenteringDirection() const {
    const Vector2 pos = getPosition();
    const auto [col, row] = worldToGridCoords(pos);
    const Vector2 anchor = gridToWorld(col, row);

    const float stepSize = getSpeed() * Stopwatch::getInstance().getDeltaTime();
    const float minCorrection = std::max(0.005f, stepSize); // World units.

    const float dx = anchor.x - pos.x;
    const float dy = anchor.y - pos.y;

    if (std::abs(dx) >= std::abs(dy)) {
        if (std::abs(dx) > minCorrection) return dx > 0.f ? Direction::Right : Direction::Left;
    } else {
        if (std::abs(dy) > minCorrection) return dy > 0.f ? Direction::Down : Direction::Up;
    }
    return Direction::None; // Already centered (within a tick's worth of movement) - nothing to correct.
}

bool Bot::tryFlee(World& world) {
    if (!isPositionDangerous(world, getPosition())) {
        fleeDirection = Direction::None; // Not fleeing (anymore) - a future flee should plan fresh.
        return false;
    }

    const auto [col, row] = worldToGridCoords(getPosition());

    if (fleeDirection != Direction::None && !isSuspectDirection(fleeDirection)) {
        const Vector2 offset = directionToVector(fleeDirection);
        const int nextCol = col + static_cast<int>(offset.x);
        const int nextRow = row + static_cast<int>(offset.y);
        if (isWalkable(world, nextCol, nextRow) && isImmediateStepSafe(world, fleeDirection)) {
            commitMovement(fleeDirection);
            return true;
        }
    }

    fleeDirection = findEscapeDirection(world, col, row);
    if (fleeDirection == Direction::None) {
        fleeDirection = recenteringDirection();
    }
    commitMovement(fleeDirection);
    return true;
}

Direction Bot::findEscapeDirection(const World& world, int startCol, int startRow) const {
    struct Node {
        int col;
        int row;
        int firstStepIndex;
    };

    std::queue<Node> frontier;
    std::set<std::pair<int, int>> visited{{startCol, startRow}};

    for (int i = 0; i < 4; ++i) {
        const int c = startCol + kGridOffsets[i].first;
        const int r = startRow + kGridOffsets[i].second;
        if (isWalkable(world, c, r) && isImmediateStepSafe(world, kGridDirections[i]) &&
            !isSuspectDirection(kGridDirections[i]) && visited.insert({c, r}).second) {
            frontier.push({c, r, i});
        }
    }

    while (!frontier.empty()) {
        const Node node = frontier.front();
        frontier.pop();

        if (!isPositionDangerous(world, gridToWorld(node.col, node.row))) {
            return kGridDirections[node.firstStepIndex];
        }

        for (int i = 0; i < 4; ++i) {
            const int c = node.col + kGridOffsets[i].first;
            const int r = node.row + kGridOffsets[i].second;
            if (isWalkable(world, c, r) && visited.insert({c, r}).second) {
                frontier.push({c, r, node.firstStepIndex});
            }
        }
    }

    return Direction::None;
}

void Bot::wander(World& world) {
    const auto [col, row] = worldToGridCoords(getPosition());

    if (wanderDirection != Direction::None && !isSuspectDirection(wanderDirection)) {
        const Vector2 offset = directionToVector(wanderDirection);
        const int nextCol = col + static_cast<int>(offset.x);
        const int nextRow = row + static_cast<int>(offset.y);
        if (isSafeToStepInto(world, nextCol, nextRow) && isImmediateStepSafe(world, wanderDirection)) {
            commitMovement(wanderDirection);
            return;
        }
    }

    std::array<Direction, 4> candidates = kGridDirections;
    for (int i = static_cast<int>(candidates.size()) - 1; i > 0; --i) {
        const int j = Random::getInstance().getInt(0, i);
        std::swap(candidates[i], candidates[j]);
    }

    for (const Direction candidate : candidates) {
        if (isSuspectDirection(candidate)) continue;
        const Vector2 offset = directionToVector(candidate);
        const int nextCol = col + static_cast<int>(offset.x);
        const int nextRow = row + static_cast<int>(offset.y);
        if (isSafeToStepInto(world, nextCol, nextRow) && isImmediateStepSafe(world, candidate)) {
            wanderDirection = candidate;
            commitMovement(candidate);
            return;
        }
    }

    wanderDirection = Direction::None;
    commitMovement(recenteringDirection());
}

bool Bot::tryCollectPowerUp(World& world) {
    constexpr int kDetectionRange = 5;
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
        chaseExactPosition(targetPosition);
        return true;
    }
    return stepToward(world, targetCol, targetRow);
}

bool Bot::tryBreakWalls(World& world) {
    if (!canPlaceBomb()) {
        return false;
    }

    const auto [col, row] = worldToGridCoords(getPosition());

    for (const auto& [dCol, dRow] : kGridOffsets) {
        if (auto wall = findWallAt(world, col + dCol, row + dRow)) {
            if (wall->isDestructible()) {
                world.placeBomb(*this);
                return true;
            }
        }
    }

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

    if (!found) return false;
    return stepToward(world, targetCol, targetRow);
}

bool Bot::tryAttack(World& world) {
    constexpr int kEngageRange = 4;
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

bool Bot::hasReachedTarget(const World& world, int col, int row, int targetCol, int targetRow) const {
    // For a walkable target (a PowerUp, a Character - neither blocks
    // movement), "reached" means standing exactly on it. For an unwalkable
    // target (a Wall, which is the whole point of tryBreakWalls()),
    // standing ON it is impossible by definition, so "reached" instead
    // means standing directly adjacent to it - the point from which the
    // wall-bombing logic takes over.
    if (isWalkable(world, targetCol, targetRow)) {
        return col == targetCol && row == targetRow;
    }
    return std::abs(col - targetCol) + std::abs(row - targetRow) == 1;
}

// Real breadth-first shortest-path search, structurally identical to
// findEscapeDirection() but searching TOWARDS a specific tile (or its
// neighbours, for an unwalkable target) instead of towards "any safe
// tile". This replaces what used to be a greedy "move along whichever
// axis has the bigger delta" heuristic, which could never detour around a
// static obstacle directly in line with the target (see the freeze bug
// described in Bot.hpp) - a real search can always find a route through
// this maze if one topologically exists, including cases where the target
// is exactly aligned with the Bot's row or column and blocked by an
// indestructible Wall pillar in between.
Direction Bot::findPathDirection(const World& world, int startCol, int startRow,
                                  int targetCol, int targetRow) const {
    if (hasReachedTarget(world, startCol, startRow, targetCol, targetRow)) {
        return Direction::None;
    }

    struct Node {
        int col;
        int row;
        int firstStepIndex;
    };

    std::queue<Node> frontier;
    std::set<std::pair<int, int>> visited{{startCol, startRow}};

    // As in findEscapeDirection(), the first hop is the one place a REAL
    // (possibly drift-affected) step needs verifying via
    // isImmediateStepSafe() - everything the BFS explores beyond it is a
    // hypothetical, perfectly-centered future cell. Also excludes any
    // direction currently on cooldown (isSuspectDirection()), so a route
    // that starts with a direction just proven not to work isn't retried
    // immediately.
    for (int i = 0; i < 4; ++i) {
        const int c = startCol + kGridOffsets[i].first;
        const int r = startRow + kGridOffsets[i].second;
        if (isSafeToStepInto(world, c, r) && isImmediateStepSafe(world, kGridDirections[i]) &&
            !isSuspectDirection(kGridDirections[i]) && visited.insert({c, r}).second) {
            frontier.push({c, r, i});
        }
    }

    // Route around danger too (not just walls/bombs), matching what the
    // old greedy code achieved via isSafeToStepInto() - a Bot chasing a
    // wall or an enemy shouldn't walk itself through an active blast to
    // get there.
    while (!frontier.empty()) {
        const Node node = frontier.front();
        frontier.pop();

        if (hasReachedTarget(world, node.col, node.row, targetCol, targetRow)) {
            return kGridDirections[node.firstStepIndex];
        }

        for (int i = 0; i < 4; ++i) {
            const int c = node.col + kGridOffsets[i].first;
            const int r = node.row + kGridOffsets[i].second;
            if (isSafeToStepInto(world, c, r) && visited.insert({c, r}).second) {
                frontier.push({c, r, node.firstStepIndex});
            }
        }
    }

    return Direction::None; // No route currently available.
}

bool Bot::stepToward(World& world, int targetCol, int targetRow) {
    const auto [col, row] = worldToGridCoords(getPosition());

    if (hasReachedTarget(world, col, row, targetCol, targetRow)) {
        commitMovement(Direction::None);
        return false; // Already there (or already adjacent, for an unwalkable target like a Wall).
    }

    const Direction direction = findPathDirection(world, col, row, targetCol, targetRow);
    if (direction != Direction::None) {
        commitMovement(direction);
        return true;
    }

    // No route found this tick - most likely every immediate neighbour is
    // currently on cooldown. Correct any drift instead of freezing
    // outright; a fresh BFS next tick, once cooldowns age down, will very
    // likely find the route this tick couldn't.
    const Direction recenter = recenteringDirection();
    commitMovement(recenter);
    return recenter != Direction::None;
}

void Bot::chaseExactPosition(Vector2 targetPosition) {
    const Vector2 myPosition = getPosition();
    const float dx = targetPosition.x - myPosition.x;
    const float dy = targetPosition.y - myPosition.y;

    constexpr float kArrivalEpsilon = 0.01f;
    if (std::abs(dx) < kArrivalEpsilon && std::abs(dy) < kArrivalEpsilon) {
        commitMovement(Direction::None);
        return;
    }

    if (std::abs(dx) >= std::abs(dy)) {
        commitMovement(dx > 0.f ? Direction::Right : Direction::Left);
    } else {
        commitMovement(dy > 0.f ? Direction::Down : Direction::Up);
    }
}

} // namespace bomberman::logic