#include "representation/Game.hpp"
#include <cstdlib>
#include <iostream>

int main() {
    // Wrapping the whole game in a try/catch means a fatal startup problem
    // (e.g. a missing/corrupt font or spritesheet file)
    try {
        bomberman::representation::Game game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}