#pragma once

#include "logic/entities/PowerUp.hpp"
#include "representation/views/EntityView.hpp"
#include <memory>
#include <array>

namespace bomberman::representation {

    enum class PowerUpSpriteVariant { Fire, ExtraBomb, Skates };

/// TODO: pick the correct icon based on the concrete PowerUp subtype (e.g.
/// dynamic_cast is explicitly discouraged, see section 3.2 - instead,
/// consider adding a `getType()`/enum accessor on PowerUp, or passing the
/// PowerUpType into this View's constructor from ConcreteFactory, which
/// already knows it).
class PowerUpView : public EntityView {
public:
    explicit PowerUpView(std::shared_ptr<const bomberman::logic::PowerUp> model,
                         std::shared_ptr<sf::Texture> texture,
                         std::array<sf::IntRect, 2> animationFrames);

    void onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) override; // TODO
    void draw(sf::RenderWindow& window, const Camera& camera) override; // TODO

    static std::array<sf::IntRect, 2> getPowerUpAnimation(PowerUpSpriteVariant variant);


private:
    std::shared_ptr<const bomberman::logic::PowerUp> model;
    sf::Sprite sprite;
    std::shared_ptr<sf::Texture> texture;
    std::array<sf::IntRect, 2> animationFrames;

    float animationTimer = 0.0f;
    std::size_t currentFrame = 0;
    static constexpr float kAnimationFrameDuration = 0.5f; // Duration for each frame in seconds
};

} // namespace bomberman::representation
