#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Character.hpp"
#include "logic/entities/PowerUp.hpp"
#include "logic/entities/Wall.hpp"
#include "logic/utils/Random.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace bomberman::logic {

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
    for (const auto& entity : entities) {
        entity->update(deltaTime);
    }

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
}

void World::placeBomb(Character& owner) {
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

    auto bomb = factory->createBomb(owner.getPosition(), ownerPtr);
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
    float yPos = -0.95f;
    const float tileSize = 0.1f;

    while (std::getline(file, line)) {
        float xPos = -0.95f;

        for (char c : line) {
            if (c == ' ') {
                xPos += tileSize;
                continue;
            }

            Vector2 pos{xPos, yPos};
            Vector2 size{tileSize * 0.9f, tileSize * 0.9f};

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

            xPos += tileSize;
        }

        yPos += tileSize;
    }
}

void World::handleCollisions() {
    // TODO: see EntityModel::intersects(). Resolve Character-vs-Wall/Bomb
    // (block movement - e.g. revert the position change from update() on
    // overlap), Character-vs-PowerUp (call PowerUp::applyEffect), and
    // Character-vs-live-explosion-tile (kill the Character).
}

void World::explode(Bomb& bomb) {
    // TODO: cross-shaped propagation in the 4 directions from bomb's
    // position, bomb.getRadius() tiles each way. Stop at the first
    // indestructible Wall; stop *after* destroying the first destructible
    // Wall found (maybe spawn a PowerUp there via Random::chance()); trigger
    // chain reactions on other Bombs found along the way
    // (Bomb::detonateEarly()); kill any Character found in the blast.
    (void)bomb;
}

} // namespace bomberman::logic
