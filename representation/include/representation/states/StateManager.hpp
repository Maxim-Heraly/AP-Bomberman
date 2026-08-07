#pragma once

#include <memory>

namespace sf { class Event; class RenderWindow; }

namespace bomberman::representation {

class State;

/// Owns the currently active State and forwards Game's calls to it.

class StateManager {
public:
    StateManager();
    ~StateManager();

    void changeState(std::unique_ptr<State> newState);

    void handleEvent(const sf::Event& event) const;
    void update(float deltaTime) const;
    void render(sf::RenderWindow& window) const;

private:
    std::unique_ptr<State> currentState;
};

} // namespace bomberman::representation
