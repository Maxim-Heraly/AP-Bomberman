#pragma once

#include "logic/patterns/AbstractFactory.hpp"
#include "logic/utils/Vector2.hpp"
#include <memory>
#include <vector>

namespace bomberman::logic {

class EntityModel;
class Character;
class Bomb;
class Wall;
class PowerUp;

/**
 * @brief Owns every EntityModel currently in the arena and orchestrates
 * their interactions. This is the biggest class in the logic library - take
 * your time designing it, and consider splitting responsibilities into
 * smaller collaborator classes (e.g. an ArenaGenerator or a CollisionSystem)
 * if World starts feeling like it's doing too much.
 *
 * TODO (section 2.1, "Game startup & Initialization"):
 *   - generateArena(): build the alternating indestructible/destructible
 *     layout, spawn the Player top-left and one Bot in each remaining corner.
 *
 * TODO (section 2.1, "Player Controls & Interactions"):
 *   - handleCollisions(): stop Characters walking through Walls/un-exploded
 *     Bombs, detect Character-vs-PowerUp and Character-vs-explosion overlap.
 *
 * TODO (section 2.1, "Bombs and Explosions"):
 *   - placeBomb(Character&): validate canPlaceBomb(), create a Bomb via factory_.
 *   - explode(Bomb&): cross-shaped propagation, stopped by indestructible
 *     Walls, stopped after exactly one destructible Wall per direction,
 *     triggers chain reactions on other Bombs in range, kills Characters in
 *     range, destroys PowerUps in range.
 *
 * TODO (section 2.1, "Enemies and AI"):
 *   - call Bot::decideNextMove(*this) for every Bot before updating them.
 */
class World {
public:
    explicit World(std::shared_ptr<AbstractFactory> factory);

    /// TODO: build the starting arena (see generateArena()) using factory_.
    void initialize();

    /// TODO: advance every entity by one tick, then resolve collisions/explosions.
    void update(float deltaTime);

    const std::vector<std::shared_ptr<EntityModel>>& getEntities() const { return entities; }
    std::shared_ptr<Character> getPlayer() const { return player; }

    /// TODO: called from PlayState when the Player presses the bomb key.
    void placeBomb(Character& owner);

    bool isGameOver() const { return gameOver; }

private:
    void generateArena();       // TODO
    void handleCollisions();    // TODO
    void explode(Bomb& bomb);   // TODO

    std::shared_ptr<AbstractFactory> factory;
    std::vector<std::shared_ptr<EntityModel>> entities;
    std::shared_ptr<Character> player;
    bool gameOver{false};
};

} // namespace bomberman::logic
