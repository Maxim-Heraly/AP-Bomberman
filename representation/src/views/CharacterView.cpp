#include "representation/views/CharacterView.hpp"

namespace bomberman::representation {

void CharacterView::onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) {
    // TODO: switch (event) to pick the right animation - advance the
    // walk-cycle frame for model_->getFacing() on Moved, switch to the
    // death animation on Died (section 2.2, "Visuals and Aesthetics").
    (void)source;
    if (event == bomberman::logic::EventType::Died) {
        markedForRemoval = true;
    }
}

namespace {
    constexpr int kFrameWidth = 16;
    constexpr int kFrameHeight = 24;
     sf::IntRect kIdleFrame{20, 47, kFrameWidth, kFrameHeight};
}

CharacterView::CharacterView(std::shared_ptr<const bomberman::logic::Character> model, std::shared_ptr<sf::Texture> texture)
    : model(std::move(model)), texture(std::move(texture)) {
    sprite.setTexture(*this->texture);
    sprite.setTextureRect(kIdleFrame);
}

void CharacterView::draw(sf::RenderWindow& window, const Camera& camera) {
    // TODO: sprite_.setPosition(...) via camera.worldToScreen(model_->getPosition()); window.draw(sprite_);
    if (!model || markedForRemoval) return;

    const auto screenPos = camera.worldToScreen(model->getPosition());
    const auto screenSize = camera.worldSizeToScreen(model->getSize());

    if (useSprite && texture) {
        sprite.setPosition(screenPos.x, screenPos.y);
        sprite.setOrigin(kFrameWidth * 0.5f, kFrameHeight * 0.5f);
        sprite.setScale(screenSize.x / kFrameWidth, screenSize.y / kFrameHeight);
        window.draw(sprite);
    }
}

} // namespace bomberman::representation
