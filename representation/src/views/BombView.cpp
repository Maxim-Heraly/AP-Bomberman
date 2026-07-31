#include "representation/views/BombView.hpp"

namespace bomberman::representation {

void BombView::onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) {
    // TODO: play the ticking animation while the bomb is alive, switch to
    // the explosion animation on BombExploded (section 2.2).
    (void)source;
    (void)event;
}

void BombView::draw(sf::RenderWindow& window, const Camera& camera) {
    // TODO: sprite_.setPosition(...) via camera.worldToScreen(model_->getPosition()); window.draw(sprite_);
    (void)window;
    (void)camera;
}

} // namespace bomberman::representation
