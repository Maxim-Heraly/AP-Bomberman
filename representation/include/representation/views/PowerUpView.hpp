#pragma once

#include "logic/entities/PowerUp.hpp"
#include "representation/views/EntityView.hpp"
#include <array>
#include <memory>

namespace bomberman::representation {

enum class PowerUpSpriteVariant { Fire, ExtraBomb, Skates };

class PowerUpView : public EntityView {
public:
    explicit PowerUpView(std::shared_ptr<const logic::PowerUp> model, std::shared_ptr<sf::Texture> texture,
                         const std::array<sf::IntRect, 2>& animationFrames);

    void onNotify(const logic::Subject& source, logic::EventType event) override;
    void draw(sf::RenderWindow& window, const Camera& camera) override;

    static std::array<sf::IntRect, 2> getPowerUpAnimation(PowerUpSpriteVariant variant);

private:
    std::shared_ptr<const logic::PowerUp> model;
    sf::Sprite sprite;
    std::shared_ptr<sf::Texture> texture;
    std::array<sf::IntRect, 2> animationFrames;

    float animationTimer = 0.0f;
    std::size_t currentFrame = 0;
    static constexpr float kAnimationFrameDuration = 0.5f; // Duration for each frame in seconds
};

} // namespace bomberman::representation
