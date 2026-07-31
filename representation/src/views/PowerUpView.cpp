#include "representation/views/PowerUpView.hpp"

namespace bomberman::representation {

void PowerUpView::onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) {
    // TODO: react to PowerUpCollected (e.g. markedForRemoval_ = true).
    (void)source;
    (void)event;
}

void PowerUpView::draw(sf::RenderWindow& window, const Camera& camera) {
    // TODO: sprite_.setPosition(...) via camera.worldToScreen(model_->getPosition()); window.draw(sprite_);
    (void)window;
    (void)camera;
}

} // namespace bomberman::representation
