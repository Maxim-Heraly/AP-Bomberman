#pragma once

#include "logic/entities/Wall.hpp"
#include "representation/views/EntityView.hpp"
#include <array>
#include <memory>

namespace bomberman::representation {

/// Selects the sprite set used to visually represent a wall.
enum class WallSpriteVariant { Destructible, Indestructible };

/// Contains the texture regions used for the wall's idle and breaking animations.
struct WallAnimationSet {
    std::array<sf::IntRect, 1> idle;
    std::array<sf::IntRect, 6> breaking;
};

/// Creates the appropriate animation frames for the requested wall variant.
WallAnimationSet makeWallAnimationSet(WallSpriteVariant variant);

class WallView : public EntityView {
public:
    /// Creates a view for a wall using the given texture and animation frames.
    WallView(std::shared_ptr<const logic::Wall> model, std::shared_ptr<sf::Texture> texture,
             const WallAnimationSet& animationSet);

    /// Starts the breaking animation when the wall receives a destruction event.
    void onNotify(const logic::Subject& source, logic::EventType event) override;

    /// Updates the breaking animation and draws the wall at its world position.
    void draw(sf::RenderWindow& window, const Camera& camera) override;

private:
    /// The wall model whose position, size, and state are represented.
    std::shared_ptr<const logic::Wall> model;

    /// SFML sprite used to render the wall.
    sf::Sprite sprite;

    /// Texture containing the wall sprites.
    std::shared_ptr<sf::Texture> texture;

    /// Texture regions used for the idle and breaking animations.
    WallAnimationSet animationSet;

    /// Whether the wall is currently playing its breaking animation.
    bool breaking{false};

    /// Elapsed time since the breaking animation started.
    float breakTimer{0.f};

    /// Total duration of the wall breaking animation in seconds.
    static constexpr float kBreakDuration{0.25f};
};

} // namespace bomberman::representation