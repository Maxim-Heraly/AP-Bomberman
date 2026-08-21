#pragma once

#include "logic/Score.hpp"
#include "logic/patterns/AbstractFactory.hpp"
#include "representation/views/EntityView.hpp"
#include <vector>

namespace bomberman::representation {

/**
 * @brief Concrete Abstract-Factory implementation that creates logic Models
 * together with their corresponding Views and connects them through the Observer pattern.
 *
 * The factory owns all created Views so that the rendering layer can retrieve
 * them through getViews(), while the World only works with logic Models.
 */
class ConcreteFactory : public logic::AbstractFactory {
public:
    /**
     * @brief Loads the textures required by the different entity Views.
     * @throws std::runtime_error if any required texture cannot be loaded.
     */
    ConcreteFactory();

    /**
     * @brief Creates a Player or Bot model and its corresponding CharacterView.
     * @param position Initial world position of the character.
     * @param isPlayer Whether the character should be a Player rather than a Bot.
     * @return The created Character model.
     */
    std::shared_ptr<logic::Character> createCharacter(logic::Vector2 position, bool isPlayer) override;

    /**
     * @brief Creates a Bomb model, its BombView, and connects the View to the model.
     * @param position Initial world position of the bomb.
     * @param owner Character that placed the bomb.
     * @return The created Bomb, or nullptr if owner is null.
     */
    std::shared_ptr<logic::Bomb> createBomb(logic::Vector2 position,
                                            std::shared_ptr<logic::Character> owner) override;

    /**
     * @brief Creates a Wall model and the corresponding WallView.
     * @param position Initial world position of the wall.
     * @param destructible Whether the wall can be destroyed by an explosion.
     * @return The created Wall model.
     */
    std::shared_ptr<logic::Wall> createWall(logic::Vector2 position, bool destructible) override;

    /**
     * @brief Creates the appropriate PowerUp model and its corresponding View.
     * @param position Initial world position of the power-up.
     * @param type Type of power-up to create.
     * @return The created PowerUp, or nullptr for an unsupported type.
     */
    std::shared_ptr<logic::PowerUp> createPowerUp(logic::Vector2 position,
                                                  logic::PowerUpType type) override;

    /**
     * @brief Returns all Views currently owned by the factory for rendering.
     */
    [[nodiscard]] const std::vector<std::shared_ptr<EntityView>>& getViews() const { return views; }

    /**
     * @brief Removes Views that have been marked for removal, typically after their model dies.
     */
    void purgeViews();

private:
    /** @brief Views owned by the factory and used by the rendering layer. */
    std::vector<std::shared_ptr<EntityView>> views;

    /** @brief Shared Score observer attached to player models. */
    std::shared_ptr<logic::Score> score = logic::Score::getInstance();

    /** @brief Texture shared by all CharacterView instances. */
    std::shared_ptr<sf::Texture> characterTexture;

    /** @brief Texture shared by bomb and wall Views. */
    std::shared_ptr<sf::Texture> arenaTexture;

    /** @brief Texture shared by all PowerUpView instances. */
    std::shared_ptr<sf::Texture> powerUpTexture;

    /** @brief Index used to cycle bot sprites between Bot1, Bot2, and Bot3. */
    std::size_t nextBotVariant{0};
};

} // namespace bomberman::representation