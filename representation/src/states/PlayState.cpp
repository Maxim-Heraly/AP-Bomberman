#include "representation/states/PlayState.hpp"
#include "representation/states/MenuState.hpp"
#include "logic/entities/Character.hpp"
#include "representation/states/StateManager.hpp"
#include "logic/utils/Stopwatch.hpp"
#include "representation/views/ScoreView.hpp"

namespace bomberman::representation {

PlayState::PlayState(StateManager& manager)
    : State(manager),
      factory(std::make_shared<ConcreteFactory>()),
      world(factory),
      camera(800, 800) { // TODO: pass in the real window size instead of hardcoding it.
    world.initialize();
}

namespace {
    bomberman::logic::Direction keyToDirection(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::Up: return bomberman::logic::Direction::Up;
            case sf::Keyboard::Down: return bomberman::logic::Direction::Down;
            case sf::Keyboard::Left: return bomberman::logic::Direction::Left;
            case sf::Keyboard::Right: return bomberman::logic::Direction::Right;
            default: return bomberman::logic::Direction::None;
        }
    }}

void PlayState::handleEvent(const sf::Event& event) {
    const auto player = world.getPlayer();
    if (!player) return;

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            world.placeBomb(*player);
            return;
        }

        const auto direction = keyToDirection(event.key.code);
        if (direction != bomberman::logic::Direction::None) {
            movementDirection = direction;
            player->setMovementInput(direction);
        }
    } else if (event.type == sf::Event::KeyReleased) {
        const auto direction = keyToDirection(event.key.code);
        if (direction != bomberman::logic::Direction::None && movementDirection == direction) {
            movementDirection = bomberman::logic::Direction::None;
            player->setMovementInput(bomberman::logic::Direction::None);
        }
    }
}

void PlayState::update(float deltaTime) {
    world.update(deltaTime);

    for (const auto& view : factory->getViews()) {
        view->update(deltaTime);
    }

    if (world.isGameOver()) {
        timer += logic::Stopwatch::getInstance().getDeltaTime();
        if (timer >= 5.0f) {
            manager.changeState(std::make_unique<MenuState>(manager));
        }
    }
}

void PlayState::render(sf::RenderWindow& window) {
    const auto windowSize = window.getSize();
    camera.setWindowSize(windowSize.x, windowSize.y);

    for (const auto& view : factory->getViews()) {
        view->draw(window, camera);
    }
    ScoreView scoreView(world.getPlayerScore());
    scoreView.draw(window, 10, 10);
}

} // namespace bomberman::representation
