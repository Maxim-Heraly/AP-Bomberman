#pragma once

#include "logic/Score.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace bomberman::representation {

    class ScoreView {
    public:
        ScoreView(std::shared_ptr<const logic::Score> score);

        void draw(sf::RenderWindow& window, int x, int y) const;

    private:
        std::shared_ptr<const logic::Score> score = logic::Score::getInstance();
        mutable sf::Text scoreText;
        sf::Font font;

        void updateText() const;
    };

} // namespace bomberman::representation