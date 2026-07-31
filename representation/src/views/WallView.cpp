#include "representation/views/WallView.hpp"

namespace bomberman::representation {

void WallView::onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) {
    // TODO: react to BlockDestroyed (e.g. a short crumble animation, then markedForRemoval_ = true).
    (void)source;
    (void)event;
}

void WallView::draw(sf::RenderWindow& window, const Camera& camera) {
    // TODO: sprite_.setPosition(...) via camera.worldToScreen(model_->getPosition()); window.draw(sprite_);
    (void)window;
    (void)camera;
}

} // namespace bomberman::representation
