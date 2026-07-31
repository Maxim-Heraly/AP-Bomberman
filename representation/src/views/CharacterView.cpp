#include "representation/views/CharacterView.hpp"

namespace bomberman::representation {

void CharacterView::onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) {
    // TODO: switch (event) to pick the right animation - advance the
    // walk-cycle frame for model_->getFacing() on Moved, switch to the
    // death animation on Died (section 2.2, "Visuals and Aesthetics").
    (void)source;
    (void)event;
}

void CharacterView::draw(sf::RenderWindow& window, const Camera& camera) {
    // TODO: sprite_.setPosition(...) via camera.worldToScreen(model_->getPosition()); window.draw(sprite_);
    (void)window;
    (void)camera;
}

} // namespace bomberman::representation
