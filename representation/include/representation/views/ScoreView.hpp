#pragma once

#include "logic/Score.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace bomberman::representation {

    class ScoreView {
    public:
        /// Creates a view for the given Score model and initializes its font and text appearance.
        ScoreView(std::shared_ptr<const logic::Score> score);

        /// Draws the current score at the specified screen position.
        void draw(sf::RenderWindow& window, int x, int y) const;

    private:
        /// Score model observed by this view; defaults to the global Score singleton.
        std::shared_ptr<const logic::Score> score = logic::Score::getInstance();

        /// SFML text object used to display the current score.
        mutable sf::Text scoreText;

        /// Font used to render the score text.
        sf::Font font;

        /// Updates the displayed text to match the current score in the model.
        void updateText() const;
    };

} // namespace bomberman::representation