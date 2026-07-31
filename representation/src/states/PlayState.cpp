#include "representation/states/PlayState.hpp"
#include "representation/states/MenuState.hpp"

namespace bomberman::representation {

PlayState::PlayState(StateManager& manager)
    : State(manager),
      factory(std::make_shared<ConcreteFactory>()),
      world(factory),
      score(std::make_shared<bomberman::logic::Score>()),
      camera(800, 800) { // TODO: pass in the real window size instead of hardcoding it.
    world.initialize();
    // TODO: attach score_ as an Observer to every relevant EntityModel - or,
    // simpler, give World a way to attach a "global" observer (e.g. to the
    // Player, plus every Wall/PowerUp as they're created) so Score doesn't
    // need to know about every entity individually.
}

void PlayState::handleEvent(const sf::Event& event) {
    // TODO: translate arrow keys to world_.getPlayer()->setMovementInput(...),
    // spacebar to world_.placeBomb(*world_.getPlayer()).
    (void)event;
}

void PlayState::update(float deltaTime) {
    world.update(deltaTime);
    // TODO: if (world_.isGameOver()) {
    //     score_->saveHighScores(...);
    //     manager_.changeState(std::make_unique<MenuState>(manager_));
    // }
}

void PlayState::render(sf::RenderWindow& window) {
    // TODO: for (const auto& view : factory_->getViews()) view->draw(window, camera_);
    (void)window;
}

} // namespace bomberman::representation
