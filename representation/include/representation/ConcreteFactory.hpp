#pragma once

#include "logic/patterns/AbstractFactory.hpp"
#include "representation/views/EntityView.hpp"
#include "logic/Score.hpp"
#include <vector>

namespace bomberman::representation {

/**
 * @brief Concrete Abstract-Factory implementation
 * for every logic Model it creates, it also creates the matching
 * View and attach()es it as an Observer to the Model (see Subject::attach).
 * Owns the Views (World only ever sees the abstract Models) so PlayState
 * can retrieve them for rendering via getViews().
 *
 */
class ConcreteFactory : public logic::AbstractFactory {
public:
    ConcreteFactory();

    std::shared_ptr<logic::Character> createCharacter(
        logic::Vector2 position, bool isPlayer) override;
    std::shared_ptr<logic::Bomb> createBomb(
        logic::Vector2 position, std::shared_ptr<logic::Character> owner) override;
    std::shared_ptr<logic::Wall> createWall(
        logic::Vector2 position, bool destructible) override;
    std::shared_ptr<logic::PowerUp> createPowerUp(
        logic::Vector2 position, logic::PowerUpType type) override;

    [[nodiscard]] const std::vector<std::shared_ptr<EntityView>>& getViews() const { return views; }

private:
    std::vector<std::shared_ptr<EntityView>> views;
    std::shared_ptr<logic::Score> score = logic::Score::getInstance();
    std::shared_ptr<sf::Texture> characterTexture;
    std::shared_ptr<sf::Texture> arenaTexture;
    std::shared_ptr<sf::Texture> powerUpTexture;
    std::size_t nextBotVariant{0}; // Used to cycle through Bot1, Bot2, Bot3 for each new Bot created.
};

} // namespace bomberman::representation
