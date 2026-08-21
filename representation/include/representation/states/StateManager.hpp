#pragma once

#include <memory>

namespace sf {
class Event;
class RenderWindow;
} // namespace sf

namespace bomberman::representation {

class State;

/**
 * @brief Manages the currently active game state.
 *
 * StateManager owns exactly one State at a time and forwards game-loop
 * operations such as event handling, updating, and rendering to that state.
 *
 * A new state can replace the current state using changeState().
 */
class StateManager {
public:
    /**
     * @brief Constructs an empty StateManager.
     *
     * No state is active when the manager is created.
     */
    StateManager();

    /**
     * @brief Destroys the StateManager and its currently active state.
     *
     * The unique_ptr automatically destroys the owned State when the
     * StateManager is destroyed.
     */
    ~StateManager();

    /**
     * @brief Replaces the currently active state with a new state.
     *
     * Ownership of newState is transferred to the StateManager. If another
     * state is currently active, it is destroyed when it is replaced.
     *
     * @param newState The new state that should become active.
     */
    void changeState(std::unique_ptr<State> newState);

    /**
     * @brief Forwards an SFML event to the active state.
     *
     * If a state is currently active, its handleEvent() function is called.
     * If no state is active, the event is ignored.
     *
     * @param event The SFML event that should be handled.
     */
    void handleEvent(const sf::Event& event) const;

    /**
     * @brief Updates the active state.
     *
     * If a state is currently active, its update() function is called with
     * the elapsed time since the previous update.
     *
     * @param deltaTime The elapsed time since the previous game update.
     */
    void update(float deltaTime) const;

    /**
     * @brief Renders the active state.
     *
     * If a state is currently active, its render() function is called.
     *
     * @param window The SFML window to render the state to.
     */
    void render(sf::RenderWindow& window) const;

private:
    /**
     * @brief The state that is currently active.
     *
     * StateManager owns the state through a unique_ptr, ensuring that the
     * state is automatically destroyed when it is replaced or when the
     * StateManager itself is destroyed.
     */
    std::unique_ptr<State> currentState;
};

} // namespace bomberman::representation