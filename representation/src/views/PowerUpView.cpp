#include "representation/views/PowerUpView.hpp"
#include "logic/utils/Stopwatch.hpp"

namespace bomberman::representation {

using logic::Stopwatch;

namespace {
constexpr int kPowerUpFrameWidth = 16;
constexpr int kPowerUpFrameHeight = 16;

sf::IntRect rect(int left, int top) { return {left, top, kPowerUpFrameWidth, kPowerUpFrameHeight}; }
} // namespace

std::array<sf::IntRect, 2> PowerUpView::getPowerUpAnimation(const PowerUpSpriteVariant variant) {
    switch (variant) {
    case PowerUpSpriteVariant::Fire:
        return {rect(16, 0), rect(16, 16)};
    case PowerUpSpriteVariant::ExtraBomb:
        return {rect(0, 0), rect(0, 16)};
    case PowerUpSpriteVariant::Skates:
        return {rect(16, 32), rect(16, 48)};
    }
    return {};
}

PowerUpView::PowerUpView(std::shared_ptr<const logic::PowerUp> model, std::shared_ptr<sf::Texture> texture,
                         const std::array<sf::IntRect, 2>& animationFrames)
    : model(std::move(model)), texture(std::move(texture)), animationFrames(animationFrames) {
    sprite.setTexture(*this->texture);
}

void PowerUpView::onNotify(const logic::Subject& /*source*/, const logic::EventType event) {
    if (event == logic::EventType::PowerUpCollected) {
        markedForRemoval = true;
    }
}

void PowerUpView::draw(sf::RenderWindow& window, const Camera& camera) {
    if (!markedForRemoval) {
        animationTimer += Stopwatch::getInstance().getDeltaTime();
        if (animationTimer >= kAnimationFrameDuration) {
            currentFrame = (currentFrame + 1) % animationFrames.size();
            animationTimer = 0.0f;
        }

        const auto screenPos = camera.worldToScreen(model->getPosition());
        const auto screenSize = camera.worldSizeToScreen(model->getSize());

        sprite.setTexture(*this->texture);
        sprite.setTextureRect(animationFrames[currentFrame]);
        sprite.setScale(screenSize.x / kPowerUpFrameWidth, screenSize.y / kPowerUpFrameHeight);
        sprite.setOrigin(kPowerUpFrameWidth * 0.5f, kPowerUpFrameHeight * 0.5f);
        sprite.setPosition(screenPos.x, screenPos.y);
        window.draw(sprite);
    }
}

} // namespace bomberman::representation
