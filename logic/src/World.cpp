#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Bot.hpp"
#include "logic/entities/Character.hpp"
#include "logic/entities/PowerUp.hpp"
#include "logic/entities/Wall.hpp"
#include "logic/utils/Random.hpp"
#include "logic/utils/Grid.hpp"
#include <algorithm>
#include <fstream>
#include <functional>
#include <sstream>
#include <unordered_set>

namespace bomberman::logic {

namespace {

    /// Allows the bomb's owner to temporarily walk through their newly placed bomb.
bool isOwnedBombPassThrough(const std::shared_ptr<Character>& character, const std::shared_ptr<Bomb>& bomb) {
    if (!bomb->canOwnerPassThrough()) {
        return false;
    }
    const auto owner = bomb->getOwner().lock();
    return owner && owner == character;
}

    /// Checks whether two axis-aligned hitboxes overlap.
bool aabbOverlap(const Vector2& posA, const Vector2& sizeA, const Vector2& posB, const Vector2& sizeB) {
    const bool overlapX = std::abs(posA.x - posB.x) < (sizeA.x + sizeB.x) * 0.5f;
    const bool overlapY = std::abs(posA.y - posB.y) < (sizeA.y + sizeB.y) * 0.5f;
    return overlapX && overlapY;
}

    /// Checks whether the character was already inside the bomb before this update,
    /// allowing them to move out of it without immediately being blocked.
bool wasAlreadyOverlappingBomb(const std::shared_ptr<Character>& character, const std::shared_ptr<Bomb>& bomb) {
    return aabbOverlap(character->getPreviousPosition(), character->getHitbox(), bomb->getPosition(), bomb->getSize());
}

    /// Converts a movement direction to the corresponding blast-profile array index.
std::size_t dirIndex(const Direction direction) {
    switch (direction) {
    case Direction::Up:
        return 0;
    case Direction::Down:
        return 1;
    case Direction::Left:
        return 2;
    case Direction::Right:
        return 3;
    case Direction::None:
        break;
    }
    return 1;
}

    /// Returns the world-space offset for moving one step in the given direction.
Vector2 stepOffset(const Direction direction, float step) {
    switch (direction) {
    case Direction::Up:
        return {0.f, -step};
    case Direction::Down:
        return {0.f, step};
    case Direction::Left:
        return {-step, 0.f};
    case Direction::Right:
        return {step, 0.f};
    case Direction::None:
        break;
    }
    return {0.f, 0.f};
}
    /// Temporary entity used to test whether a particular tile is reached by a blast.
struct BlastTile : EntityModel {
    BlastTile(const Vector2 position, const Vector2 size) : EntityModel(position, size) {}
    void update(float /*deltaTime*/) override {}
};
} // namespace

World::World(std::shared_ptr<AbstractFactory> factory) : factory(std::move(factory)) {}

void World::initialize() { generateArena(); }

void World::update(const float deltaTime) {
    // While the game is running, update the score timer and let bots choose
    // their next actions before the entities themselves are updated.
    if (!gameOver) {
        tickTimer += deltaTime;

        if (tickTimer >= 1.f) {
            score->addTick();
            tickTimer -= 1.f;
        }
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
    }

    for (const auto& entity : entities) {
        entity->update(deltaTime);
    }

    handleCollisions();

    // Collect exploded bombs first because exploding them may add or remove
    // entities from the world.
    std::vector<std::shared_ptr<Bomb>> explodedBombs;
    explodedBombs.reserve(entities.size());
    for (const auto& entity : entities) {
        auto bomb = std::dynamic_pointer_cast<Bomb>(entity);
        if (bomb && bomb->hasExploded()) {
            explodedBombs.push_back(bomb);
        }
    }

    for (const auto& bomb : explodedBombs) {
        if (!gameOver)
            explode(*bomb);
    }

    // Remove entities that were destroyed during collision handling or explosions.
    entities.erase(std::remove_if(entities.begin(), entities.end(),
                                  [](const std::shared_ptr<EntityModel>& entity) { return !entity->isAlive(); }),
                   entities.end());

    // Check the game result only after all deaths from this update have been processed.
    if (!gameOver && !player->isAlive()) {
        player->declareLoss();
        gameOver = true;
    }

    if (!gameOver && player->isAlive()) {
        bool anyBotsAlive = false;
        for (const auto& entity : entities) {
            auto character = std::dynamic_pointer_cast<Character>(entity);
            if (character && character.get() != player.get() && character->isAlive()) {
                anyBotsAlive = true;
                break;
            }
        }
        if (!anyBotsAlive) {
            player->declareWin();
            gameOver = true;
        }
    }
}

void World::placeBomb(Character& owner) {
    if (!owner.isAlive())
        return;
    if (!owner.tryPlaceBomb())
        return;

    // Resolve the owner back to the shared_ptr stored by the World so the bomb
    // can keep a weak reference to the actual Character object.
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

    // Snap the bomb to the center of the nearest arena tile.
    Vector2 pos = owner.getPosition();
    pos.x = -1.0f + (std::round((pos.x + 1.0f - kTileWidth * 0.5f) / kTileWidth) + 0.5f) * kTileWidth;
    pos.y = -1.0f + (std::round((pos.y + 1.0f - kTileHeight * 0.5f) / kTileHeight) + 0.5f) * kTileHeight;

    const auto bomb = factory->createBomb(pos, ownerPtr);
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


    std::string line;
    float yPos = -1.0f + kTileHeight * 0.5f;

    while (std::getline(file, line)) {
        float xPos = -1.0f + kTileWidth * 0.5f;

        for (char c : line) {
            if (c == ' ') {
                xPos += kTileWidth;
                continue;
            }

            Vector2 pos{xPos, yPos};

            switch (c) {
            case 'W': {
                auto wall = factory->createWall(pos, false);
                if (wall)
                    entities.push_back(wall);
                break;
            }
            case 'D': {
                if (Random::getInstance().chance(0.75f)) {
                    auto wall = factory->createWall(pos, true);
                    if (wall)
                        entities.push_back(wall);
                }
                break;
            }
            case 'P': {
                if (!player) {
                    player = factory->createCharacter(pos, true);
                    if (player)
                        entities.push_back(player);
                }
                break;
            }
            case 'B': {
                auto bot = factory->createCharacter(pos, false);
                if (bot)
                    entities.push_back(bot);
                break;
            }
            default:
                break;
            }

            xPos += kTileWidth;
        }

        yPos += kTileHeight;
    }
}

void World::handleCollisions() const {
    // Separate entities by type so collision checks only consider relevant objects.
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
        } else if (auto powerUp = std::dynamic_pointer_cast<PowerUp>(entity)) {
            powerUps.push_back(std::move(powerUp));
        }
    }

    for (const auto& bomb : bombs) {
        if (!bomb->canOwnerPassThrough())
            continue;
        const auto owner = bomb->getOwner().lock();
        if (!owner)
            continue;
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
        // Keep track of bombs already processed to prevent chain reactions from
    // recursively processing the same bomb more than once.
    const auto start =
        std::find_if(entities.begin(), entities.end(), [&bomb](const std::shared_ptr<EntityModel>& entity) {
            auto candidate = std::dynamic_pointer_cast<Bomb>(entity);
            return candidate && candidate.get() == &bomb;
        });

    if (start == entities.end())
        return;

    const auto startBomb = std::dynamic_pointer_cast<Bomb>(*start);
    if (!startBomb)
        return;

    const auto owner = startBomb->getOwner().lock();
    const bool ownerIsPlayer = owner && owner == player;

    // Keep track of bombs already processed to prevent chain reactions from
    // recursively processing the same bomb more than once.
    std::unordered_set<const Bomb*> processedBombs;

    std::function<void(const std::shared_ptr<Bomb>&)> explodeBomb = [&](const std::shared_ptr<Bomb>& currentBomb) {
        if (!currentBomb || !processedBombs.insert(currentBomb.get()).second)
            return;

        Bomb::BlastProfile profile{};
        const Vector2 center = currentBomb->getPosition();
        const Vector2 stepSize = currentBomb->getSize();
        const int radius = std::max(1, currentBomb->getRadius());

        // Trace the blast one tile at a time until its radius is reached
        // or an obstacle stops it.
        for (const Direction direction : {Direction::Up, Direction::Down, Direction::Left, Direction::Right}) {
            const std::size_t index = dirIndex(direction);
            int visibleReach = 0;
            bool hasNaturalEnd = false;

            for (int step = 1; step <= radius; ++step) {
                const Vector2 pos = center + stepOffset(direction, stepSize.x * static_cast<float>(step));
                BlastTile blastTile{pos, stepSize};

                bool blockedByWall = false;

                for (const auto& entity : entities) {
                    if (!entity || entity.get() == currentBomb.get())
                        continue;
                    if (!entity->intersects(blastTile))
                        continue;

                    // If the blast hits a destructible wall, destroy it and possibly spawn a power-up.
                    if (const auto wall = std::dynamic_pointer_cast<Wall>(entity)) {
                        if (wall->isDestructible()) {
                            wall->destroy();
                            if (ownerIsPlayer) {
                                owner->declareBlockDestroyed();
                            }
                            if (Random::getInstance().chance(0.2f)) {
                                const auto chance = Random::getInstance().getInt(1, 3);
                                PowerUpType type;
                                if (chance == 1) {
                                    type = PowerUpType::Fire;
                                }
                                if (chance == 2) {
                                    type = PowerUpType::ExtraBomb;
                                }
                                if (chance == 3) {
                                    type = PowerUpType::Skates;
                                }
                                auto powerUp = factory->createPowerUp(wall->getPosition(), type);
                                if (powerUp) {
                                    entities.push_back(powerUp);
                                }
                            }
                        }
                        blockedByWall = true;
                        break;
                    }
                    // If the blast hits another bomb, detonate it immediately and recursively
                    if (const auto otherBomb = std::dynamic_pointer_cast<Bomb>(entity)) {
                        if (!otherBomb->hasExploded()) {
                            otherBomb->detonateEarly();
                            otherBomb->update(0.f);
                        }
                        explodeBomb(otherBomb);
                        continue;
                    }
                    // If the blast hits a Character, kill them.
                    if (const auto character = std::dynamic_pointer_cast<Character>(entity)) {
                        if (character != player && ownerIsPlayer) {
                            character->declareEnemyKilled();
                        }
                        character->die();
                        continue;
                    }
                    // If the blast hits a PowerUp, remove it from the world.
                    if (const auto powerUp = std::dynamic_pointer_cast<PowerUp>(entity)) {
                        powerUp->remove();
                        continue;
                    }
                }
                if (blockedByWall) {
                    break;
                }
                // Store the calculated blast profile so the representation layer can
                // render the correct explosion tiles and endpoints.
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
