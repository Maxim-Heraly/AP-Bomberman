#pragma once

#include "representation/states/StateManager.hpp"
#include <SFML/Graphics.hpp>

namespace bomberman::representation {

/**
 * @brief Entry point of the representation layer: owns the sf::RenderWindow
 * and the main game loop. Everything game-logic related is delegated to
 * StateManager / the current State - see section 3.1, "Game" class
 * description.
 *
 * TODO: implement run() as the classic
 *   while (window_.isOpen()) { processEvents(); update(dt); render(); }
 * loop, calling bomberman::logic::Stopwatch::getInstance().tick() once at
 * the top of each iteration and using getDeltaTime() for update(). Consider
 * window_.setFramerateLimit(60) to cap the framerate (explicitly allowed as
 * an exception to "no busy-waiting", see section 3.1 "Stopwatch") rather
 * than any manual sleep.
 */
class Game {
public:
    Game();

    void run(); // TODO

private:
    void processEvents();          // TODO
    void update(float deltaTime);
    void render();                 // TODO

    sf::RenderWindow window;
    StateManager stateManager;
};

} // namespace bomberman::representation
