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
 * @brief Owns every EntityModel currently in the arena and orchestrates
 * their interactions.
 *
 * TODO (section 2.1, "Enemies and AI"):
 *   - call Bot::decideNextMove(*this) for every Bot before updating them.
 */
class World {
public:
    explicit World(std::shared_ptr<AbstractFactory> factory);

    void initialize();

    void update(float deltaTime);

    [[nodiscard]] const std::vector<std::shared_ptr<EntityModel>>& getEntities() const { return entities; }
    [[nodiscard]] std::shared_ptr<Character> getPlayer() const { return player; }

    void placeBomb(Character& owner);

    [[nodiscard]] bool isGameOver() const { return gameOver; }

private:
    void generateArena();
    void handleCollisions() const;
    void explode(Bomb& bomb);

    std::shared_ptr<AbstractFactory> factory;
    std::vector<std::shared_ptr<EntityModel>> entities;
    std::shared_ptr<Character> player;
    std::shared_ptr<Score> score = Score::getInstance();
    bool gameOver{false};
    float tickTimer{0.f};
};

} // namespace bomberman::logic
