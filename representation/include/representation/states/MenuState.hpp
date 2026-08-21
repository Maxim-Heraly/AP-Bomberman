#pragma once

#include "logic/Score.hpp"
#include "representation/states/State.hpp"

namespace bomberman::representation {

/**
 * @brief Represents the main menu state of the game.
 *
 * MenuState is responsible for displaying the game's main menu,
 * including the title, high scores, and PLAY button.
 *
 * It also handles mouse input for starting a new game.
 */
class MenuState : public State {
public:
    using State::State;

    /**
     * @brief Constructs the menu state.
     *
     * Loads the saved high scores and starts the menu background music.
     *
     * @param manager Reference to the StateManager responsible for
     *                switching between game states.
     */
    explicit MenuState(StateManager& manager);

    /**
     * @brief Handles events received while the menu is active.
     *
     * Checks for a left mouse-button click. If the click occurs
     * inside the PLAY button, the current score is reset and the
     * game switches to the PlayState.
     *
     * @param event SFML event that needs to be processed.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Updates the menu state.
     *
     * The menu is static, so there is currently no per-frame logic
     * that needs to be performed.
     *
     * @param deltaTime Time elapsed since the previous frame, in seconds.
     */
    void update(float deltaTime) override;

    /**
     * @brief Draws the menu to the game window.
     *
     * Renders the game title, top-five scores, PLAY button,
     * and keyboard controls hint. The PLAY button position is
     * recalculated every frame so that it remains centered when
     * the window size changes.
     *
     * @param window SFML render window onto which the menu is drawn.
     */
    void render(sf::RenderWindow& window) override;

private:
    /**
     * @brief Shared access to the game's Score object.
     *
     * The Score singleton is used here to load and display the
     * current top-five high scores. The menu does not modify
     * individual high-score entries.
     */
    std::shared_ptr<logic::Score> score =
        logic::Score::getInstance();

    /**
     * @brief Bounding rectangle of the PLAY button.
     *
     * This rectangle is recalculated during render() and is later
     * used by handleEvent() to determine whether the mouse click
     * occurred inside the PLAY button.
     */
    sf::FloatRect playButtonBounds;
};

} // namespace bomberman::representation