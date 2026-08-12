#include "logic/entities/Bot.hpp"
#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Wall.hpp"
#include "logic/entities/PowerUp.hpp"
#include "logic/utils/Grid.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace bomberman::logic {

namespace {
    // Bot's own movement/detection heuristics need to reason about the same
    // grid World::generateArena() actually lays tiles out on. The columns
    // (15) and rows (13) don't divide [-1, 1] into equal steps, so X and Y
    // need separate step sizes - a single shared "kTileStep" previously
    // silently drifted away from the real spacing (particularly on the Y
    // axis), which made the Bot misjudge which tiles were blocked/in range.
    constexpr float kEpsilonX = kTileWidth * 0.5f;
    constexpr float kEpsilonY = kTileHeight * 0.5f;
    constexpr float kAvgTileStep = (kTileWidth + kTileHeight) * 0.5f;
    constexpr float kPowerUpDetectionRange = 6.f * kAvgTileStep;
    constexpr float kEnemyCloseRange = 3.f * kAvgTileStep;

    constexpr std::array<Direction, 4> kDirections{
        Direction::Up, Direction::Down, Direction::Left, Direction::Right};

    Vector2 stepOffset(Direction direction) {
        switch (direction) {
            case Direction::Up:    return {0.f, -kTileHeight};
            case Direction::Down:  return {0.f, kTileHeight};
            case Direction::Left:  return {-kTileWidth, 0.f};
            case Direction::Right: return {kTileWidth, 0.f};
            case Direction::None:  break;
        }
        return {0.f, 0.f};
    }

    // Snaps a position to the center of whichever tile it currently sits
    // in - the same rounding World::placeBomb() uses. A Bot only ever
    // moves along one axis at a time (see Character::update), so the
    // *other* axis never gets a chance to self-correct: a Bot that moved
    // vertically for a while and then turns to move horizontally keeps
    // whatever fractional Y offset it had. That's fine for the physics,
    // but it's poison for a *one-tile-ahead* candidate check - corridors
    // here are almost exactly as wide as the Bot's own body, so a few
    // ticks of accumulated drift is enough for a "one tile over" guess to
    // clip a diagonally-adjacent pillar that a perfectly-centered Bot
    // would have cleared, making every direction look blocked. Anchoring
    // candidate generation to the intended tile center avoids that.
    Vector2 roundToTileCenter(const Vector2& pos) {
        const float x = -1.f + (std::round((pos.x + 1.f - kTileWidth * 0.5f) / kTileWidth) + 0.5f) * kTileWidth;
        const float y = -1.f + (std::round((pos.y + 1.f - kTileHeight * 0.5f) / kTileHeight) + 0.5f) * kTileHeight;
        return {x, y};
    }

    float squaredDistance(const Vector2& a, const Vector2& b) {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    // True axis-aligned overlap test, mirroring EntityModel::intersects().
    // isTileBlocked() used to approximate this with a "same tile center,
    // within half a tile" epsilon check - but that epsilon (~half a tile)
    // is much *smaller* than the actual collision threshold the physics
    // uses (half the Bot's width/height plus half the obstacle's, which is
    // close to a *full* tile). Since Bots move continuously rather than
    // snapping to the grid, they're almost always slightly off-center, and
    // that gap let the Bot judge a tile "clear" while World::handleCollisions
    // still reverted the move as a real collision - so a chosen "escape"
    // direction could silently fail every single tick.
    bool wouldOverlap(const Vector2& posA, const Vector2& sizeA, const Vector2& posB, const Vector2& sizeB) {
        const bool overlapX = std::abs(posA.x - posB.x) < (sizeA.x + sizeB.x) * 0.5f;
        const bool overlapY = std::abs(posA.y - posB.y) < (sizeA.y + sizeB.y) * 0.5f;
        return overlapX && overlapY;
    }
}

bool Bot::isTileBlocked(const World& world, const Vector2& tile) const {
    for (const auto& entity : world.getEntities()) {
        if (!entity->isAlive()) continue;
        if (auto wall = std::dynamic_pointer_cast<Wall>(entity)) {
            if (wouldOverlap(tile, size, wall->getPosition(), wall->getSize())) return true;
        } else if (auto bomb = std::dynamic_pointer_cast<Bomb>(entity)) {
            if (wouldOverlap(tile, size, bomb->getPosition(), bomb->getSize())) return true;
        }
    }
    return false;
}

bool Bot::isInLineWithinRange(const Vector2& from, const Vector2& target, int range) const {
    const bool sameRow = std::abs(from.y - target.y) < kEpsilonY;
    const bool sameCol = std::abs(from.x - target.x) < kEpsilonX;
    if (!sameRow && !sameCol) return false;

    // Distance is measured along whichever axis the blast actually travels:
    // a shared row means the blast reaches along X (tiles spaced kTileWidth
    // apart), a shared column means it reaches along Y (kTileHeight apart).
    if (sameRow) {
        const float distance = std::abs(from.x - target.x);
        return distance <= kTileWidth * static_cast<float>(range) + kEpsilonX;
    }
    const float distance = std::abs(from.y - target.y);
    return distance <= kTileHeight * static_cast<float>(range) + kEpsilonY;
}

bool Bot::isTileDangerous(const World& world, const Vector2& tile) const {
    for (const auto& entity : world.getEntities()) {
        auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
        if (!bomb || bomb->hasExploded()) continue;
        if (isInLineWithinRange(tile, bomb->getPosition(), bomb->getRadius())) return true;
    }
    return false;
}

bool Bot::isEnemyClose(const World& world) const {
    for (const auto& entity : world.getEntities()) {
        auto character = std::dynamic_pointer_cast<Character>(entity);
        if (!character || character.get() == this || !character->isAlive()) continue;
        if (squaredDistance(position, character->getPosition()) <= kEnemyCloseRange * kEnemyCloseRange) return true;
    }
    return false;
}

bool Bot::moveTowards(World& world, const Vector2& target) {
    const float dx = target.x - position.x;
    const float dy = target.y - position.y;
    if (std::abs(dx) < kEpsilonX && std::abs(dy) < kEpsilonY) return false; // Already there.

    const Vector2 anchor = roundToTileCenter(position);

    // Rank all four directions by how much each would reduce the remaining
    // distance to the target (most direct first). A fixed "primary, then
    // secondary" pair isn't enough: whenever the target is exactly on one
    // axis (very common - e.g. lining up with a wall or an enemy to bomb
    // it), there IS no secondary axis, so a single blocked direction left
    // the Bot with nothing else to try and it just froze. Ranking all four
    // lets it sidestep around a pillar instead - essential in this
    // checkerboard maze, where a straight line to the target is rarely
    // walkable.
    std::array<Direction, 4> ranked = kDirections;
    std::sort(ranked.begin(), ranked.end(), [&](Direction a, Direction b) {
        auto progress = [&](Direction dir) {
            switch (dir) {
                case Direction::Up:    return -dy;
                case Direction::Down:  return dy;
                case Direction::Left:  return -dx;
                case Direction::Right: return dx;
                default: return -std::numeric_limits<float>::max();
            }
        };
        return progress(a) > progress(b);
    });

    for (Direction direction : ranked) {
        const Vector2 candidate = anchor + stepOffset(direction);
        if (isTileBlocked(world, candidate)) continue;
        if (isTileDangerous(world, candidate)) continue; // Don't detour through a live blast.

        setMovementInput(direction);
        return true;
    }
    return false;
}

bool Bot::tryFlee(World& world) {
    if (!isTileDangerous(world, position)) return false;

    const Vector2 anchor = roundToTileCenter(position);
    Direction best = Direction::None;
    float bestScore = -std::numeric_limits<float>::max();

    for (Direction direction : kDirections) {
        const Vector2 candidate = anchor + stepOffset(direction);
        if (isTileBlocked(world, candidate)) continue;

        const bool safe = !isTileDangerous(world, candidate);

        float nearestBombDistSq = std::numeric_limits<float>::max();
        for (const auto& entity : world.getEntities()) {
            auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
            if (!bomb || bomb->hasExploded()) continue;
            nearestBombDistSq = std::min(nearestBombDistSq, squaredDistance(candidate, bomb->getPosition()));
        }

        // Prefer candidates that still have somewhere further to go from
        // them. Without this, a Bot that flees into a pocket bounded by
        // walls on most sides - including its own just-placed bomb, which
        // seals the way it came from the moment it steps off it - has no
        // way to tell that tile apart from open ground: it walks in, finds
        // every exit blocked, and just sits there until the bomb that's
        // still "in range" of it goes off.
        int openExits = 0;
        for (Direction next : kDirections) {
            if (!isTileBlocked(world, candidate + stepOffset(next))) ++openExits;
        }

        const float score = (safe ? 10000.f : 0.f) + openExits * 1000.f + nearestBombDistSq;
        if (score > bestScore) {
            bestScore = score;
            best = direction;
        }
    }

    setMovementInput(best);
    return true;
}

bool Bot::tryCollectPowerUp(World& world) {
    std::shared_ptr<PowerUp> nearest;
    float nearestDistSq = kPowerUpDetectionRange * kPowerUpDetectionRange;

    for (const auto& entity : world.getEntities()) {
        auto powerUp = std::dynamic_pointer_cast<PowerUp>(entity);
        if (!powerUp || !powerUp->isAlive()) continue;
        const float distSq = squaredDistance(position, powerUp->getPosition());
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = powerUp;
        }
    }
    if (!nearest) return false;

    return moveTowards(world, nearest->getPosition());
}

bool Bot::tryBreakWalls(World& world) {
    std::shared_ptr<Wall> nearest;
    float nearestDistSq = std::numeric_limits<float>::max();

    for (const auto& entity : world.getEntities()) {
        auto wall = std::dynamic_pointer_cast<Wall>(entity);
        if (!wall || !wall->isAlive() || !wall->isDestructible()) continue;
        const float distSq = squaredDistance(position, wall->getPosition());
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            nearest = wall;
        }
    }
    if (!nearest) return false; // None left - tryAttack() takes over.

    if (canPlaceBomb() && isInLineWithinRange(position, nearest->getPosition(), getBombRadius())) {
        world.placeBomb(*this);
        return true; // tryFlee() steers the Bot away from its own bomb next tick.
    }

    return moveTowards(world, nearest->getPosition());
}

bool Bot::tryAttack(World& world) {
    std::shared_ptr<Character> target;
    float nearestDistSq = std::numeric_limits<float>::max();

    for (const auto& entity : world.getEntities()) {
        auto character = std::dynamic_pointer_cast<Character>(entity);
        if (!character || character.get() == this || !character->isAlive()) continue;
        const float distSq = squaredDistance(position, character->getPosition());
        if (distSq < nearestDistSq) {
            nearestDistSq = distSq;
            target = character;
        }
    }
    if (!target) return false;

    if (canPlaceBomb() && isInLineWithinRange(position, target->getPosition(), getBombRadius())) {
        world.placeBomb(*this);
        return true;
    }

    return moveTowards(world, target->getPosition());
}

void Bot::decideNextMove(World& world) {
    if (!isAlive()) {
        setMovementInput(Direction::None);
        return;
    }

    if (tryFlee(world)) return;
    if (tryCollectPowerUp(world)) return;

    // "...or an enemy is close" - go for the kill instead of detouring to
    // a wall when someone's right on top of us.
    if (isEnemyClose(world) && tryAttack(world)) return;
    if (tryBreakWalls(world)) return;
    if (tryAttack(world)) return;

    setMovementInput(Direction::None);
}

} // namespace bomberman::logic