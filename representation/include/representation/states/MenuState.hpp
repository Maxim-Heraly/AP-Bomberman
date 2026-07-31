#pragma once

#include "logic/Score.hpp"
#include "representation/states/State.hpp"

namespace bomberman::representation {

    /**
     * @brief TODO: display the top-5 scoreboard (Score::getTopFive(), loaded via
     * Score::loadHighScores() when this state is entered - section 2.1, "Game
     * startup & Initialization") and a "Play" button. On click,
     * manager_.changeState(std::make_unique<PlayState>(manager_)).
     */
    class MenuState : public State {
    public:
        using State::State;

        explicit MenuState(StateManager& manager);

        void handleEvent(const sf::Event& event) override;
        void update(float deltaTime) override;
        void render(sf::RenderWindow& window) override;

    private:
    bomberman::logic::Score score; // Used read-only here, just to load and display the top-5.
    sf::FloatRect playButtonBounds; // Recomputed every render() call, used by handleEvent() to detect clicks.
};

} // namespace bomberman::representation
