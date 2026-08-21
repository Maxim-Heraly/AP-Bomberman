#pragma once

#include "logic/Score.hpp"
#include "logic/World.hpp"
#include "logic/utils/Direction.hpp"
#include "representation/Camera.hpp"
#include "representation/ConcreteFactory.hpp"
#include "representation/states/State.hpp"
#include <memory>

namespace bomberman::representation {

/**
 * @brief Represents the main gameplay state of the application.
 *
 * PlayState owns and coordinates all components required for one game run:
 * the game logic (World), graphical views (ConcreteFactory), camera,
 * score system, and gameplay input.
 */
class PlayState : public State {
public:
    /**
     * @brief Constructs a new gameplay state.
     *
     * Initializes the factory, world, score system, and camera.
     * The game world is initialized and the gameplay music is started.
     *
     * @param manager The StateManager responsible for switching between states.
     */
    explicit PlayState(StateManager& manager);

    /**
     * @brief Handles user input during gameplay.
     *
     * Keyboard input is converted into movement commands for the player.
     * The Space key places a bomb. Movement input is also cleared when
     * the corresponding key is released.
     *
     * Input is ignored once the game is over.
     *
     * @param event The SFML event to process.
     */
    void handleEvent(const sf::Event& event) override;

    /**
     * @brief Updates the gameplay state.
     *
     * Updates the game world and all graphical views. When the game ends,
     * the appropriate victory or loss sound is played and a timer is started.
     * After the result has been displayed for a few seconds, the score is
     * saved, the music is stopped, and the state returns to the menu.
     *
     * @param deltaTime Time elapsed since the previous update, in seconds.
     */
    void update(float deltaTime) override;

    /**
     * @brief Renders the current gameplay state.
     *
     * Draws the game views according to their draw layers, followed by the
     * player's score. When the game is over, a centered "YOU WIN!" or
     * "GAME OVER" message is displayed.
     *
     * @param window The SFML window in which the game is rendered.
     */
    void render(sf::RenderWindow& window) override;

private:
    /**
     * @brief Factory responsible for creating and managing graphical views.
     *
     * The factory creates the representation objects corresponding to
     * entities in the game world.
     */
    std::shared_ptr<ConcreteFactory> factory;

    /**
     * @brief The logical game world.
     *
     * Contains the game entities and manages gameplay logic such as
     * movement, bombs, collisions, enemies, and game-over detection.
     */
    logic::World world;

    /**
     * @brief Shared score system used during the game.
     *
     * Keeps track of the player's score and is also used to save the
     * high scores when the game ends.
     */
    std::shared_ptr<logic::Score> score;

    /**
     * @brief Camera used to convert world coordinates to screen coordinates.
     */
    Camera camera;

    /**
     * @brief Direction currently controlled by the player.
     *
     * Used to remember which movement key is currently active so that
     * movement can be stopped when that key is released.
     */
    logic::Direction movementDirection{logic::Direction::None};

    /**
     * @brief Timer used to delay the transition back to the menu after game over.
     *
     * The timer starts when the game ends and allows the result message
     * and sound to remain visible/audible for a short period.
     */
    float timer{0.0f};

    /**
     * @brief Prevents the game-over sound from being played multiple times.
     *
     * Since update() is called every frame, this flag ensures that the
     * victory or loss sound is triggered only once.
     */
    bool resultSoundPlayed{false};
};

} // namespace bomberman::representation