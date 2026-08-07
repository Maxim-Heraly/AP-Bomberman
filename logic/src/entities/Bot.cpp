#include "logic/entities/Bot.hpp"
#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Wall.hpp"
#include "logic/entities/PowerUp.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace bomberman::logic {

namespace {
    // Matches the grid spacing World::generateArena() lays tiles out on
    // (tile centers at -0.95, -0.85, ..., 0.95 - see World.cpp).
    constexpr float kTileStep = 0.1f;
    // Half a tile: generous enough to absorb the fact that Characters move
    // continuously rather than snapping to the grid.
    constexpr float kEpsilon = kTileStep * 0.5f;
    constexpr float kPowerUpDetectionRange = 6.f * kTileStep;
    constexpr float kEnemyCloseRange = 3.f * kTileStep;

    constexpr std::array<Direction, 4> kDirections{
        Direction::Up, Direction::Down, Direction::Left, Direction::Right};

    Vector2 stepOffset(Direction direction, float step) {
        switch (direction) {
            case Direction::Up:    return {0.f, -step};
            case Direction::Down:  return {0.f, step};
            case Direction::Left:  return {-step, 0.f};
            case Direction::Right: return {step, 0.f};
            case Direction::None:  break;
        }
        return {0.f, 0.f};
    }

    float squaredDistance(const Vector2& a, const Vector2& b) {
        const float dx = a.x - b.x;
        const float dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    bool isSameTile(const Vector2& a, const Vector2& b) {
        return std::abs(a.x - b.x) < kEpsilon && std::abs(a.y - b.y) < kEpsilon;
    }
}

bool Bot::isTileBlocked(const World& world, const Vector2& tile) const {
    for (const auto& entity : world.getEntities()) {
        if (!entity->isAlive()) continue;
        if (auto wall = std::dynamic_pointer_cast<Wall>(entity)) {
            if (isSameTile(wall->getPosition(), tile)) return true;
        } else if (auto bomb = std::dynamic_pointer_cast<Bomb>(entity)) {
            if (isSameTile(bomb->getPosition(), tile)) return true;
        }
    }
    return false;
}

bool Bot::isInLineWithinRange(const Vector2& from, const Vector2& target, int range) const {
    const bool sameRow = std::abs(from.y - target.y) < kEpsilon;
    const bool sameCol = std::abs(from.x - target.x) < kEpsilon;
    if (!sameRow && !sameCol) return false;

    const float distance = sameRow ? std::abs(from.x - target.x) : std::abs(from.y - target.y);
    return distance <= kTileStep * static_cast<float>(range) + kEpsilon;
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

Direction Bot::directionTowards(const Vector2& target) const {
    const float dx = target.x - position.x;
    const float dy = target.y - position.y;
    if (std::abs(dx) < kEpsilon && std::abs(dy) < kEpsilon) return Direction::None;

    // Only one cardinal direction can be requested at a time - close the
    // bigger gap first, the other axis catches up over the next ticks.
    if (std::abs(dx) > std::abs(dy)) {
        return dx > 0.f ? Direction::Right : Direction::Left;
    }
    return dy > 0.f ? Direction::Down : Direction::Up;
}

bool Bot::tryFlee(World& world) {
    if (!isTileDangerous(world, position)) return false;

    Direction best = Direction::None;
    float bestScore = -std::numeric_limits<float>::max();

    for (Direction direction : kDirections) {
        const Vector2 candidate = position + stepOffset(direction, kTileStep);
        if (isTileBlocked(world, candidate)) continue;

        const bool safe = !isTileDangerous(world, candidate);

        float nearestBombDistSq = std::numeric_limits<float>::max();
        for (const auto& entity : world.getEntities()) {
            auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
            if (!bomb || bomb->hasExploded()) continue;
            nearestBombDistSq = std::min(nearestBombDistSq, squaredDistance(candidate, bomb->getPosition()));
        }

        // Genuinely safe tiles always beat merely-unblocked ones; among
        // ties, prefer whichever ends up furthest from the nearest bomb.
        const float score = (safe ? 1000.f : 0.f) + nearestBombDistSq;
        if (score > bestScore) {
            bestScore = score;
            best = direction;
        }
    }

    setMovementInput(best); // Stays None if truly boxed in.
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

    const Direction direction = directionTowards(nearest->getPosition());
    if (direction == Direction::None) return false; // Already overlapping it.
    if (isTileBlocked(world, position + stepOffset(direction, kTileStep))) return false;

    setMovementInput(direction);
    return true;
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

    const Direction direction = directionTowards(nearest->getPosition());
    if (direction == Direction::None) return false;
    if (isTileBlocked(world, position + stepOffset(direction, kTileStep))) return false;

    setMovementInput(direction);
    return true;
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

    const Direction direction = directionTowards(target->getPosition());
    if (direction == Direction::None) return false;
    if (isTileBlocked(world, position + stepOffset(direction, kTileStep))) return false;

    setMovementInput(direction);
    return true;
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