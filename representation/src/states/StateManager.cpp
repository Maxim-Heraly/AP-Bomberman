#include "representation/states/StateManager.hpp"
#include "representation/states/State.hpp"

namespace bomberman::representation {

StateManager::StateManager() = default;
StateManager::~StateManager() = default;

void StateManager::changeState(std::unique_ptr<State> newState) {
    currentState = std::move(newState);
}

void StateManager::handleEvent(const sf::Event& event) {
    if (currentState) currentState->handleEvent(event);
}

void StateManager::update(float deltaTime) {
    if (currentState) currentState->update(deltaTime);
}

void StateManager::render(sf::RenderWindow& window) {
    if (currentState) currentState->render(window);
}

} // namespace bomberman::representation
