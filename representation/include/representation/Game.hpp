#pragma once

#include "representation/states/StateManager.hpp"
#include <SFML/Graphics.hpp>

namespace bomberman::representation {

/**
 * @brief Entry point of the representation layer: owns the sf::RenderWindow
 * and the main game loop. Everything game-logic related is delegated to
 * StateManager / the current State
 */
class Game {
public:
    Game();

    void run();

private:
    void processEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window;
    StateManager stateManager;
};

} // namespace bomberman::representation
