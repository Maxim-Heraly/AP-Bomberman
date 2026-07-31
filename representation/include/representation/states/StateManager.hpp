#pragma once

#include <memory>

namespace sf { class Event; class RenderWindow; }

namespace bomberman::representation {

class State;

/// Owns the currently active State and forwards Game's calls to it.

class StateManager {
public:
    StateManager();
    ~StateManager(); // Declared (defined = default in the .cpp) so unique_ptr<State> works with an incomplete State here.

    void changeState(std::unique_ptr<State> newState);

    void handleEvent(const sf::Event& event);
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

private:
    std::unique_ptr<State> currentState;
};

} // namespace bomberman::representation
