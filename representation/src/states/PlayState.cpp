#include "representation/states/PlayState.hpp"
#include "representation/states/MenuState.hpp"

namespace bomberman::representation {

PlayState::PlayState(StateManager& manager)
    : State(manager),
      factory(std::make_shared<ConcreteFactory>()),
      world(factory),
      score(std::make_shared<bomberman::logic::Score>()),
      camera(800, 800) { // TODO: pass in the real window size instead of hardcoding it.
    world.initialize();
    demoPlayer = factory->createCharacter({0.f, 0.f}, true);
    // TODO: attach score_ as an Observer to every relevant EntityModel - or,
    // simpler, give World a way to attach a "global" observer (e.g. to the
    // Player, plus every Wall/PowerUp as they're created) so Score doesn't
    // need to know about every entity individually.
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
    if (!demoPlayer) return;

    if (event.type == sf::Event::KeyPressed) {
        const auto direction = keyToDirection(event.key.code);
        if (direction != bomberman::logic::Direction::None) {
            movementDirection = direction;
            demoPlayer->setMovementInput(direction);
        }
    } else if (event.type == sf::Event::KeyReleased) {
        const auto direction = keyToDirection(event.key.code);
        if (direction != bomberman::logic::Direction::None && movementDirection == direction) {
            movementDirection = bomberman::logic::Direction::None;
            demoPlayer->setMovementInput(bomberman::logic::Direction::None);
        }
    }
}

void PlayState::update(float deltaTime) {
    if (demoPlayer) {
        demoPlayer->update(deltaTime);
    }
    world.update(deltaTime);
    // TODO: if (world_.isGameOver()) {
    //     score_->saveHighScores(...);
    //     manager_.changeState(std::make_unique<MenuState>(manager_));
    // }
}

void PlayState::render(sf::RenderWindow& window) {
    const auto windowSize = window.getSize();
    camera.setWindowSize(windowSize.x, windowSize.y);

    for (const auto& view : factory->getViews()) {
        view->draw(window, camera);
    }
}

} // namespace bomberman::representation
