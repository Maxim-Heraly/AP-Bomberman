#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Character.hpp"
#include "logic/entities/PowerUp.hpp"
#include "logic/entities/Wall.hpp"
#include <algorithm>

namespace bomberman::logic {

World::World(std::shared_ptr<AbstractFactory> factory) : factory(std::move(factory)) {}

void World::initialize() {
    generateArena();
    if (!player) {
        player = factory->createCharacter({-0.9f, -0.9f}, true); // Player top-left
        entities.push_back(player);
    }
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
    // TODO: see section 2.1 "Game startup & Initialization" - alternating
    // indestructible blocks (fixed grid positions) filled with destructible
    // blocks (low chance of leaving air instead), Player spawned top-left,
    // one Bot per remaining corner. Use factory_->createWall(...) /
    // factory_->createCharacter(...) and Random::getInstance() for the
    // destructible-vs-air roll. Remember to keep the four spawn corners clear.
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
