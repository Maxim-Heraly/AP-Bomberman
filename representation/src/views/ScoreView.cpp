#include "representation/views/ScoreView.hpp"

namespace bomberman::representation {

    ScoreView::ScoreView(std::shared_ptr<const logic::Score> score)
        : score(std::move(score)) {
        if (!font.loadFromFile(ASSET_DIR "/fonts/PublicPixel-rv0pA.ttf")) {
            throw std::runtime_error("Failed to load font");
        }
        scoreText.setFont(font);
        scoreText.setCharacterSize(20);
        scoreText.setFillColor(sf::Color::White);
        updateText();
    }

    void ScoreView::updateText() const {
        if (score) {
            scoreText.setString("Score: " + std::to_string(score->getCurrentScore()));
        }
    }

    void ScoreView::draw(sf::RenderWindow& window, int x, int y) const {
        updateText();
        scoreText.setPosition(static_cast<float>(x), static_cast<float>(y));
        window.draw(scoreText);
    }

} // namespace bomberman::representation