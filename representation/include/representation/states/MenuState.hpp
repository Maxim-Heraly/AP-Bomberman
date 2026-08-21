#pragma once

#include "logic/Score.hpp"
#include "representation/states/State.hpp"

namespace bomberman::representation {

class MenuState : public State {
public:
    using State::State;

    explicit MenuState(StateManager& manager);

    void handleEvent(const sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    std::shared_ptr<logic::Score> score =
        logic::Score::getInstance(); // Used read-only here, just to load and display the top-5.
    sf::FloatRect playButtonBounds;  // Recomputed every render() call, used by handleEvent() to detect clicks.
};

} // namespace bomberman::representation
