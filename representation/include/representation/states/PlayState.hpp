#pragma once

#include "logic/Score.hpp"
#include "logic/World.hpp"
#include "logic/utils/Direction.hpp"
#include "representation/Camera.hpp"
#include "representation/ConcreteFactory.hpp"
#include "representation/states/State.hpp"
#include <memory>


namespace bomberman::representation {

/**
 * @brief Owns one "run" of the game: a World, its ConcreteFactory, a Camera
 * and a Score. TODO:
 *   - handleEvent(): translate arrow-key / spacebar presses into
 *     world_.getPlayer()->setMovementInput(...) / world_.placeBomb(...)
 *     calls - this class should only know *that* an action was requested,
 *     not *how* it affects the World (section 3.1, "Game" description).
 *   - update(): world_.update(deltaTime); if (world_.isGameOver()) persist
 *     the score and manager_.changeState(...) back to a MenuState.
 *   - render(): draw every EntityView from factory_->getViews().
 */
class PlayState : public State {
public:
    explicit PlayState(StateManager& manager);

    void handleEvent(const sf::Event& event) override; // TODO
    void update(float deltaTime) override;              // TODO
    void render(sf::RenderWindow& window) override;     // TODO

private:
    std::shared_ptr<ConcreteFactory> factory;
    bomberman::logic::World world;
    std::shared_ptr<bomberman::logic::Score> score;
    Camera camera;
    bomberman::logic::Direction movementDirection{bomberman::logic::Direction::None};
    float timer{0.0f};
};

} // namespace bomberman::representation
