#include "representation/states/PlayState.hpp"
#include "representation/states/MenuState.hpp"
#include "logic/entities/Character.hpp"
#include "representation/states/StateManager.hpp"
#include "logic/utils/Stopwatch.hpp"
#include "representation/views/ScoreView.hpp"
#include "logic/Score.hpp"

namespace bomberman::representation {

PlayState::PlayState(StateManager& manager)
    : State(manager),
      score(logic::Score::getInstance()),
      factory(std::make_shared<ConcreteFactory>()),
      world(factory),
      camera(900, 690) {
    world.initialize();
}

namespace {
    logic::Direction keyToDirection(sf::Keyboard::Key key) {
        switch (key) {
            case sf::Keyboard::Up: return logic::Direction::Up;
            case sf::Keyboard::Down: return logic::Direction::Down;
            case sf::Keyboard::Left: return logic::Direction::Left;
            case sf::Keyboard::Right: return logic::Direction::Right;
            default: return logic::Direction::None;
        }
    }}

void PlayState::handleEvent(const sf::Event& event) {
    const auto player = world.getPlayer();
    if (!player || world.isGameOver()) return;

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space) {
            world.placeBomb(*player);
            return;
        }

        const auto direction = keyToDirection(event.key.code);
        if (direction != logic::Direction::None) {
            movementDirection = direction;
            player->setMovementInput(direction);
        }
    } else if (event.type == sf::Event::KeyReleased) {
        const auto direction = keyToDirection(event.key.code);
        if (direction != logic::Direction::None && movementDirection == direction) {
            movementDirection = logic::Direction::None;
            player->setMovementInput(logic::Direction::None);
        }
    }
}

void PlayState::update(float deltaTime) {
    world.update(deltaTime);

    for (const auto& view : factory->getViews()) {
        view->update(deltaTime);
    }

    if (world.isGameOver()) {
        timer += deltaTime;
        if (timer >= 5.0f) {
            score->saveHighScores("../../highscores.txt");
            manager.changeState(std::make_unique<MenuState>(manager));
        }
    }
}

void PlayState::render(sf::RenderWindow& window) {
    const auto windowSize = window.getSize();
    camera.setWindowSize(windowSize.x, windowSize.y);

    for (const auto& view : factory->getViews()) {
        view->draw(window, camera);
    }
    ScoreView scoreView(logic::Score::getInstance());
    scoreView.draw(window, 10, 10);

    // only show when game is over
    if (!world.isGameOver()) {
        return;
    }

    sf::Font font;
    if (!font.loadFromFile("../../assets/fonts/PublicPixel-rv0pA.ttf")) {
        std::printf("Failed to load font\n");
        return;
    }

    const float centerX = static_cast<float>(windowSize.x) * 0.5f;
    const float centerY = static_cast<float>(windowSize.y) * 0.5f;
    const auto player = world.getPlayer();
    const bool won = player && player->isAlive();
    sf::Text resultText(won ? "YOU WIN!" : "GAME OVER", font, 48);
    resultText.setFillColor(won ? sf::Color::Yellow : sf::Color::Red);
    resultText.setStyle(sf::Text::Bold);
    const auto bounds = resultText.getLocalBounds();
    resultText.setOrigin(bounds.left + bounds.width * 0.5f, bounds.top + bounds.height * 0.5f);
    resultText.setPosition(centerX, centerY);
    window.draw(resultText);
}

} // namespace bomberman::representation
