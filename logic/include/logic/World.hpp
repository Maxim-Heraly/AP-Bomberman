#pragma once

#include "Score.hpp"
#include "logic/patterns/AbstractFactory.hpp"
#include <memory>
#include <vector>

namespace bomberman::logic {

class EntityModel;
class Character;
class Bomb;
class Wall;
class PowerUp;
class Score;

/**
 * @brief Owns and updates all entities in the game world.
 *
 * Handles arena generation, entity updates, collisions, bomb explosions,
 * scoring, and determining when the game has ended.
 */
class World {
public:
    /**
     * @brief Creates a World using the given factory to construct entities.
     * @param factory Factory used to create characters, walls, bombs and power-ups.
     */
    explicit World(std::shared_ptr<AbstractFactory> factory);

    /**
     * @brief Initializes the world by generating the arena and its entities.
     */
    void initialize();

    /**
     * @brief Updates all entities, handles collisions, explosions and game state.
     * @param deltaTime Time elapsed since the previous update in seconds.
     */
    void update(float deltaTime);

    /**
     * @brief Returns all entities currently present in the world.
     * @return Read-only reference to the world's entity collection.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<EntityModel>>& getEntities() const { return entities; }

    /**
     * @brief Returns the player character.
     * @return Shared pointer to the player, or nullptr if none exists.
     */
    [[nodiscard]] std::shared_ptr<Character> getPlayer() const { return player; }

    /**
     * @brief Attempts to place a bomb for the given character.
     *
     * The bomb is positioned at the center of the character's current arena tile.
     */
    void placeBomb(Character& owner);

    /**
     * @brief Checks whether the game has finished.
     * @return True if either the player has won or lost.
     */
    [[nodiscard]] bool isGameOver() const { return gameOver; }

private:
    /**
     * @brief Loads the arena layout from the arena file and creates its entities.
     */
    void generateArena();

    /**
     * @brief Resolves character interactions with walls, bombs and power-ups.
     *
     * Characters that collide with blocking objects are moved back to their previous position.
     */
    void handleCollisions() const;

    /**
     * @brief Calculates a bomb's blast in all four directions and applies its effects.
     *
     * Destructible walls, characters, power-ups and other bombs encountered by the blast are handled here.
     */
    void explode(Bomb& bomb);

    /// Factory used to create all entities belonging to the world.
    std::shared_ptr<AbstractFactory> factory;

    /// All entities currently active in the world.
    std::vector<std::shared_ptr<EntityModel>> entities;

    /// The human-controlled player character.
    std::shared_ptr<Character> player;

    /// Observer responsible for tracking and persisting the player's score.
    std::shared_ptr<Score> score = Score::getInstance();

    /// Indicates whether the game has ended.
    bool gameOver{false};

    /// Accumulates elapsed time until the next score tick is awarded.
    float tickTimer{0.f};
};

} // namespace bomberman::logic