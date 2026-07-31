#pragma once

#include <SFML/Graphics.hpp>

namespace bomberman::representation {

class StateManager;

/// Base class for the State pattern used to separate the menu/scoreboard
/// screen from actual gameplay (section 2.1 "Game startup & Initialization",
/// and section 3.1's "Game" description: "Other responsibilities may be
/// delegated to the StateManager or concrete States").
class State {
public:
    explicit State(StateManager& manager) : manager(manager) {}
    virtual ~State() = default;

    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

protected:
    StateManager& manager;
};

} // namespace bomberman::representation
