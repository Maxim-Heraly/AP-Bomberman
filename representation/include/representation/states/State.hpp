#pragma once

#include <SFML/Graphics.hpp>

namespace bomberman::representation {

class StateManager;

class State {
public:
    /**
     * @brief Constructs a State and associates it with a StateManager.
     *
     * The StateManager is stored as a reference so that concrete states
     * can request state changes when necessary.
     *
     * @param manager The StateManager responsible for managing this state.
     */
    explicit State(StateManager& manager) : manager(manager) {}

    /**
     * @brief Virtual destructor for the State base class.
     *
     * Ensures that derived State objects are correctly destroyed when
     * accessed through a State base-class pointer.
     */
    virtual ~State() = default;

    /**
     * @brief Handles an SFML event.
     *
     * Concrete states implement this function to react to user input
     * and other window events.
     *
     * @param event The SFML event to handle.
     */
    virtual void handleEvent(const sf::Event& event) = 0;

    /**
     * @brief Updates the state.
     *
     * Concrete states implement this function to update their logic,
     * timers, animations, and other time-dependent behaviour.
     *
     * @param deltaTime The time elapsed since the previous update, in seconds.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Renders the state to the given window.
     *
     * Concrete states implement this function to draw their contents,
     * such as the menu, scoreboard, or gameplay scene.
     *
     * @param window The SFML render window to draw to.
     */
    virtual void render(sf::RenderWindow& window) = 0;

protected:
    /// Reference to the StateManager that owns and controls this state.
    StateManager& manager;
};

} // namespace bomberman::representation