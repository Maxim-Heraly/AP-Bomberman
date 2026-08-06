#pragma once

#include "logic/patterns/AbstractFactory.hpp"
#include "representation/views/EntityView.hpp"
#include "logic/Score.hpp"
#include <vector>

namespace bomberman::representation {

/**
 * @brief Concrete Abstract-Factory implementation (section 3.1.1, "Abstract
 * Factory"): for every logic Model it creates, it also creates the matching
 * View and attach()es it as an Observer to the Model (see Subject::attach).
 * Owns the Views (World only ever sees the abstract Models) so PlayState /
 * Game can retrieve them for rendering via getViews().
 *
 * TODO: implement each create*() override:
 *   1. Construct the Model (shared_ptr<...>).
 *   2. Construct the matching View, passing it the Model.
 *   3. model->attach(view) so the Model can notify it.
 *   4. Store the View in views_ (see getViews()).
 *   5. Return the Model.
 *
 * TODO: periodically prune views_ of entries where isMarkedForRemoval() is
 * true (e.g. once per frame from PlayState, after death animations finish
 * playing) so the view list doesn't grow forever.
 */
class ConcreteFactory : public bomberman::logic::AbstractFactory {
public:
    ConcreteFactory();

    std::shared_ptr<bomberman::logic::Character> createCharacter(
        bomberman::logic::Vector2 position, bool isPlayer) override;
    std::shared_ptr<bomberman::logic::Bomb> createBomb(
        bomberman::logic::Vector2 position, std::shared_ptr<bomberman::logic::Character> owner) override;
    std::shared_ptr<bomberman::logic::Wall> createWall(
        bomberman::logic::Vector2 position, bool destructible) override;
    std::shared_ptr<bomberman::logic::PowerUp> createPowerUp(
        bomberman::logic::Vector2 position, bomberman::logic::PowerUpType type) override;

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
