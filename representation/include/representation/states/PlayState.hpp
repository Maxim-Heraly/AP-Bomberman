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
 * and a Score.
 */
class PlayState : public State {
public:
    explicit PlayState(StateManager& manager);

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    std::shared_ptr<ConcreteFactory> factory;
    logic::World world;
    std::shared_ptr<logic::Score> score;
    Camera camera;
    logic::Direction movementDirection{logic::Direction::None};
    float timer{0.0f};
    bool resultSoundPlayed{false};
};

} // namespace bomberman::representation
