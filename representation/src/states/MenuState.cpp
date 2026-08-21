#include "representation/states/MenuState.hpp"
#include "representation/AudioManager.hpp"
#include "representation/states/PlayState.hpp"
#include "representation/states/StateManager.hpp"
#include <sstream>

namespace bomberman::representation {

namespace {
constexpr float ButtonWidth = 200.f;
constexpr float ButtonHeight = 60.f;
} // namespace

MenuState::MenuState(StateManager& manager) : State(manager) {
    score->loadHighScores("../../highscores.txt");
    AudioManager::getInstance().playMenuMusic();
}

void MenuState::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::MouseButtonPressed || event.mouseButton.button != sf::Mouse::Left) {
        return;
    }
    const auto click = sf::Vector2f(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
    if (playButtonBounds.contains(click)) {
        score->setCurrentScore(0);
        manager.changeState(std::make_unique<PlayState>(manager));
    }
}

void MenuState::update(float /*deltaTime*/) {
    // The menu is static - nothing needs to change here every frame.
}

void MenuState::render(sf::RenderWindow& window) {
    const sf::Vector2u windowSize = window.getSize();
    const float centerX = static_cast<float>(windowSize.x) * 0.5f;
    sf::Font font;
    if (!font.loadFromFile("../../assets/fonts/PublicPixel-rv0pA.ttf")) {
        std::printf("Failed to load font\n");
    }
    sf::Text title("BOMBERMAN", font, 42);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setOrigin(title.getLocalBounds().width * 0.5f, 0.f);
    title.setPosition(centerX, 40.f);
    window.draw(title);

    sf::Text subtitle("Top 5 Scores", font, 22);
    subtitle.setFillColor(sf::Color(200, 200, 200));
    subtitle.setOrigin(subtitle.getLocalBounds().width * 0.5f, 0.f);
    subtitle.setPosition(centerX, 115.f);
    window.draw(subtitle);

    float y = 155.f;
    const auto& topFive = score->getTopFive();
    if (topFive.empty()) {
        sf::Text none("No scores yet - be the first!", font, 18);
        none.setFillColor(sf::Color(160, 160, 160));
        none.setOrigin(none.getLocalBounds().width * 0.5f, 0.f);
        none.setPosition(centerX, y);
        window.draw(none);
        y += 32.f;
    } else {
        int rank = 1;
        for (const auto& entry : topFive) {
            std::ostringstream line;
            line << rank << ". " << entry.name << "  -  " << entry.score;
            sf::Text text(line.str(), font, 20);
            text.setFillColor(sf::Color::White);
            text.setOrigin(text.getLocalBounds().width * 0.5f, 0.f);
            text.setPosition(centerX, y);
            window.draw(text);
            y += 30.f;
            ++rank;
        }
    }

    // Laid out fresh every frame (rather than cached once) so it stays
    // centered even if the window size were ever to change.
    playButtonBounds = sf::FloatRect(centerX - ButtonWidth * 0.5f, y + 30.f, ButtonWidth, ButtonHeight);
    sf::RectangleShape button({ButtonWidth, ButtonHeight});
    button.setPosition(playButtonBounds.left, playButtonBounds.top);
    button.setFillColor(sf::Color(70, 170, 90));
    window.draw(button);

    sf::Text buttonText("PLAY", font, 26);
    buttonText.setStyle(sf::Text::Bold);
    buttonText.setFillColor(sf::Color::White);
    const auto textBounds = buttonText.getLocalBounds();
    buttonText.setOrigin(textBounds.left + textBounds.width * 0.5f, textBounds.top + textBounds.height * 0.5f);
    buttonText.setPosition(playButtonBounds.left + ButtonWidth * 0.5f, playButtonBounds.top + ButtonHeight * 0.5f);
    window.draw(buttonText);

    sf::Text hint("Arrow keys to move, Space to place a bomb", font, 14);
    hint.setFillColor(sf::Color(150, 150, 150));
    hint.setOrigin(hint.getLocalBounds().width * 0.5f, 0.f);
    hint.setPosition(centerX, static_cast<float>(windowSize.y) - 30.f);
    window.draw(hint);
}

} // namespace bomberman::representation