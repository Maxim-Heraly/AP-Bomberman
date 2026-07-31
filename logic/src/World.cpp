#include "logic/World.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Character.hpp"
#include "logic/entities/PowerUp.hpp"
#include "logic/entities/Wall.hpp"

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
    (void)deltaTime;
}

void World::placeBomb(Character& owner) {
    // TODO: if (owner.canPlaceBomb()) factory_->createBomb(owner position, owner);
    // and store the resulting shared_ptr<Bomb> in entities_.
    (void)owner;
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
