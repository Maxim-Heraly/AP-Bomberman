#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Character.hpp"
#include "logic/entities/Bot.hpp"
#include "logic/entities/PowerUp.hpp"
#include "logic/entities/Wall.hpp"
#include "logic/utils/Random.hpp"
#include "logic/utils/Grid.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <functional>
#include <unordered_set>


namespace bomberman::logic {

namespace {
    bool isOwnedBombPassThrough( const std::shared_ptr<Character>& character, const std::shared_ptr<Bomb>& bomb) {
        if (!bomb->canOwnerPassThrough()) {
            return false;
        }
        const auto owner = bomb->getOwner().lock();
        return owner && owner == character;
    }

    bool aabbOverlap(const Vector2& posA, const Vector2& sizeA, const Vector2& posB, const Vector2& sizeB) {
        const bool overlapX = std::abs(posA.x - posB.x) < (sizeA.x + sizeB.x) * 0.5f;
        const bool overlapY = std::abs(posA.y - posB.y) < (sizeA.y + sizeB.y) * 0.5f;
        return overlapX && overlapY;
    }

    bool wasAlreadyOverlappingBomb(const std::shared_ptr<Character>& character, const std::shared_ptr<Bomb>& bomb) {
        return aabbOverlap(character->getPreviousPosition(), character->getSize(),
                           bomb->getPosition(), bomb->getSize());
    }

    std::size_t dirIndex(Direction direction) {
        switch (direction) {
            case Direction::Up: return 0;
            case Direction::Down: return 1;
            case Direction::Left: return 2;
            case Direction::Right: return 3;
            case Direction::None: break;
        }
        return 1;
    }

    Vector2 stepOffset(Direction direction, float step) {
        switch (direction) {
            case Direction::Up: return {0.f, -step};
            case Direction::Down: return {0.f, step};
            case Direction::Left: return {-step, 0.f};
            case Direction::Right: return {step, 0.f};
            case Direction::None: break;
        }
        return {0.f, 0.f};
    }

    struct BlastTile : EntityModel {
        BlastTile(Vector2 position, Vector2 size) : EntityModel(position, size) {}
        void update(float /*deltaTime*/) override {}
    };
}

World::World(std::shared_ptr<AbstractFactory> factory) : factory(std::move(factory)) {}

void World::initialize() {
    generateArena();
}

void World::update(float deltaTime) {
    // TODO, suggested order:
    // 1. for each Bot in entities_: decideNextMove(*this)
    // 2. for each entity: entity->update(deltaTime)
    // 3. handleCollisions()
    // 4. for each Bomb with hasExploded(): explode(*bomb)
    // 5. erase-remove every entity with !isAlive() from entities_
    // 6. update gameOver_ (Player died, or Player is the last Character standing)
    if (!gameOver) {
        std::vector<std::shared_ptr<Bot>> bots;
        bots.reserve(entities.size());
        for (const auto& entity : entities) {
            if (auto bot = std::dynamic_pointer_cast<Bot>(entity)) {
                bots.push_back(std::move(bot));
            }
        }
        for (const auto& bot : bots) {
            if (bot->isAlive()) {
                bot->decideNextMove(*this);
            }
        }

        for (const auto& entity : entities) {
            entity->update(deltaTime);
        }

        handleCollisions();

        std::vector<std::shared_ptr<Bomb>> explodedBombs;
        explodedBombs.reserve(entities.size());
        for (const auto& entity : entities) {
            auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
            if (bomb && bomb->hasExploded()) {
                explodedBombs.push_back(bomb);
            }
        }

        for (const auto& bomb : explodedBombs) {
            explode(*bomb);
        }

        entities.erase(
            std::remove_if(entities.begin(), entities.end(),
                [](const std::shared_ptr<EntityModel>& entity) { return !entity->isAlive(); }),
            entities.end());

        if (!gameOver && !player->isAlive()) {
            score->addPlayerLost();
            gameOver = true;
        }

        if (! gameOver && player->isAlive()) {
            bool anyBotsAlive = false;
            for (const auto& entity : entities) {
                auto character = std::dynamic_pointer_cast<Character>(entity);
                if (character && character.get() != player.get() && character->isAlive()) {
                    anyBotsAlive = true;
                    break;
                }
            }
            if (!anyBotsAlive) {
                score->addPlayerWon();
                gameOver = true;
            }
        }
    }
}

void World::placeBomb(Character& owner) {
    if (!owner.isAlive()) return;
    if (!owner.tryPlaceBomb()) return;

    std::shared_ptr<Character> ownerPtr;
    if (player && player.get() == &owner) {
        ownerPtr = player;
    } else {
        for (const auto& entity : entities) {
            auto character = std::dynamic_pointer_cast<Character>(entity);
            if (character && character.get() == &owner) {
                ownerPtr = std::move(character);
                break;
            }
        }
    }

    if (!ownerPtr) {
        owner.onBombExploded();
        return;
    }

    constexpr float tileWidth = 2.0f / 15.0f;
    constexpr float tileHeight = 2.0f / 13.0f;

    Vector2 pos = owner.getPosition();
    pos.x = -1.0f + (std::round((pos.x + 1.0f - tileWidth * 0.5f) / tileWidth) + 0.5f) * tileWidth;
    pos.y = -1.0f + (std::round((pos.y + 1.0f - tileHeight * 0.5f) / tileHeight) + 0.5f) * tileHeight;

    auto bomb = factory->createBomb(pos, ownerPtr);
    if (!bomb) {
        owner.onBombExploded();
        return;
    }

    entities.push_back(bomb);
}

void World::generateArena() {
    std::ifstream file("../../assets/arenas/normal_arena.txt");
    if (!file.is_open()) {
        throw std::runtime_error("Failed to load arena.txt");
    }

    // Grid is 15 columns x 13 rows and must
    // exactly fill [-1, 1] on both axes - so tile width/height are derived
    // from the grid size rather than hardcoded, and can differ from each
    // other since the world doesn't need square tiles (Camera stretches
    // world space to the window independently on each axis anyway).
    constexpr int kColumns = 15;
    constexpr int kRows = 13;
    constexpr float tileWidth = 2.0f / kColumns;
    constexpr float tileHeight = 2.0f / kRows;

    std::string line;
    float yPos = -1.0f + tileHeight * 0.5f;

    while (std::getline(file, line)) {
        float xPos = -1.0f + tileWidth * 0.5f;

        for (char c : line) {
            if (c == ' ') {
                xPos += tileWidth;
                continue;
            }

            Vector2 pos{xPos, yPos};

            switch (c) {
                case 'W': {
                    auto wall = factory->createWall(pos, false);
                    if (wall) entities.push_back(wall);
                    break;
                }
                case 'D': {
                    if (Random::getInstance().chance(0.75f)) {
                        auto wall = factory->createWall(pos, true);
                        if (wall) entities.push_back(wall);
                    }
                    break;
                }
                case 'P': {
                    if (!player) {
                        player = factory->createCharacter(pos, true);
                        if (player) entities.push_back(player);
                    }
                    break;
                }
                case 'B': {
                    auto bot = factory->createCharacter(pos, false);
                    if (bot) entities.push_back(bot);
                    break;
                }
                default:
                    break;
            }

            xPos += tileWidth;
        }

        yPos += tileHeight;
    }
}

void World::handleCollisions() const {
    std::vector<std::shared_ptr<Character>> characters;
    std::vector<std::shared_ptr<Wall>> walls;
    std::vector<std::shared_ptr<Bomb>> bombs;
    std::vector<std::shared_ptr<PowerUp>> powerUps;

    for (const auto& entity : entities) {
        if (auto character = std::dynamic_pointer_cast<Character>(entity)) {
            characters.push_back(std::move(character));
        } else if (auto wall = std::dynamic_pointer_cast<Wall>(entity)) {
            walls.push_back(std::move(wall));
        } else if (auto bomb = std::dynamic_pointer_cast<Bomb>(entity)) {
            bombs.push_back(std::move(bomb));
        }
        else if (auto powerUp = std::dynamic_pointer_cast<PowerUp>(entity)) {
            powerUps.push_back(std::move(powerUp));
        }
    }

    for (const auto& bomb : bombs) {
        if (!bomb->canOwnerPassThrough()) continue;
        auto owner = bomb->getOwner().lock();
        if (!owner) continue;
        if (!owner->intersects(*bomb)) {
            bomb->disableOwnerPassThrough();
        }
    }

    for (const auto& character : characters) {
        bool blocked = false;

        for (const auto& powerUp : powerUps) {
            if (character->intersects(*powerUp)) {
                powerUp->applyEffect(*character);
                if (character == player) {
                    character->collectPowerUp();
                }
            }
        }

        for (const auto& wall : walls) {
            if (character->intersects(*wall)) {
                blocked = true;
                break;
            }
        }

        if (!blocked) {
            for (const auto& bomb : bombs) {
                if (isOwnedBombPassThrough(character, bomb)) {
                    continue;
                }
                if (wasAlreadyOverlappingBomb(character, bomb)) {
                    continue;
                }

                if (character->intersects(*bomb)) {
                    blocked = true;
                    break;
                }
            }
        }

        if (blocked) {
            character->revertToPreviousPosition();
        }
    }
}

void World::explode(Bomb& bomb) {
    auto start = std::find_if(entities.begin(), entities.end(),
        [&bomb](const std::shared_ptr<EntityModel>& entity) {
            auto candidate = std::dynamic_pointer_cast<Bomb>(entity);
            return candidate && candidate.get() == &bomb;
        });

    if (start == entities.end()) return;

    auto startBomb = std::dynamic_pointer_cast<Bomb>(*start);
    if (!startBomb) return;

    auto owner = startBomb->getOwner().lock();
    bool ownerIsPlayer = owner && owner == player;

    std::unordered_set<const Bomb*> processedBombs;

    std::function<void(const std::shared_ptr<Bomb>&)> explodeBomb =
        [&](const std::shared_ptr<Bomb>& currentBomb) {
            if (!currentBomb || !processedBombs.insert(currentBomb.get()).second) return;

            Bomb::BlastProfile profile{};
            const Vector2 center = currentBomb->getPosition();
            const Vector2 stepSize = currentBomb->getSize();
            const int radius = std::max(1, currentBomb->getRadius());

            for (Direction direction : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
                const std::size_t index = dirIndex(direction);
                int visibleReach = 0;
                bool hasNaturalEnd = false;

                for (int step = 1; step <= radius; ++step) {
                    const Vector2 pos = center + stepOffset(direction, stepSize.x * static_cast<float>(step));
                    BlastTile blastTile{pos, stepSize};

                    bool blockedByWall = false;

                    for (const auto& entity : entities) {
                        if (!entity || entity.get() == currentBomb.get()) continue;
                        if (!entity->intersects(blastTile)) continue;

                        if (auto wall = std::dynamic_pointer_cast<Wall>(entity)) {
                            if (wall->isDestructible()) {
                                wall->destroy();
                                if (ownerIsPlayer) {
                                    owner->declareBlockDestroyed();
                                }
                                if (Random::getInstance().chance(0.33f)) {
                                    auto chance = Random::getInstance().getInt(1,3);
                                    PowerUpType type;
                                    if (chance == 1) { type = PowerUpType::Fire;}
                                    if (chance == 2) { type = PowerUpType::ExtraBomb; }
                                    if (chance == 3) { type = PowerUpType::Skates; }
                                    auto powerUp = factory->createPowerUp(wall->getPosition(), type);
                                    if (powerUp) {
                                        entities.push_back(powerUp);
                                    }
                                }
                            }
                            blockedByWall = true;
                            break;
                        }
                        if (auto otherBomb = std::dynamic_pointer_cast<Bomb>(entity)) {
                            if (!otherBomb->hasExploded()) {
                                otherBomb->detonateEarly();
                                otherBomb->update(0.f);
                            }
                            explodeBomb(otherBomb);
                            continue;
                        }
                        if (auto character = std::dynamic_pointer_cast<Character>(entity)) {
                            if (character != player && ownerIsPlayer) {
                                character->declareEnemyKilled();
                            }
                            character->die();
                            continue;
                        }
                        if (auto powerUp = std::dynamic_pointer_cast<PowerUp>(entity)) {
                            powerUp->remove();
                            continue;
                        }
                    }
                    if (blockedByWall) {
                        break;
                    }
                    visibleReach = step;
                    hasNaturalEnd = (step == radius);
                }
                profile.reach[index] = visibleReach;
                profile.hasEnd[index] = hasNaturalEnd && visibleReach > 0;
            }
            currentBomb->setBlastProfile(profile);
    };
    explodeBomb(startBomb);
}

} // namespace bomberman::logic
