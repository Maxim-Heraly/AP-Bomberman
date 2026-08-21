#include "representation/Game.hpp"
#include "logic/utils/Stopwatch.hpp"
#include "representation/states/MenuState.hpp"

namespace bomberman::representation {

Game::Game() : window(sf::VideoMode(900, 690), "Bomberman", sf::Style::Titlebar | sf::Style::Close) {
    window.setFramerateLimit(60);
    stateManager.changeState(std::make_unique<MenuState>(stateManager));
}

void Game::run() {
    while (window.isOpen()) {
        logic::Stopwatch::getInstance().tick();
        processEvents();
        update(logic::Stopwatch::getInstance().getDeltaTime());
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            continue;
        }
        stateManager.handleEvent(event);
    }
}

void Game::update(float deltaTime) const { stateManager.update(deltaTime); }

void Game::render() {
    window.clear(sf::Color(27, 61, 42)); // A dark neutral background behind the arena.
    stateManager.render(window);
    window.display();
}

} // namespace bomberman::representation
