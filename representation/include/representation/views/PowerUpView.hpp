#pragma once

#include "logic/entities/PowerUp.hpp"
#include "representation/views/EntityView.hpp"
#include <array>
#include <memory>

namespace bomberman::representation {

/// Identifies which power-up spritesheet animation should be used.
enum class PowerUpSpriteVariant { Fire, ExtraBomb, Skates };

class PowerUpView : public EntityView {
public:
    /**
     * @brief Creates a view for a power-up using the given texture and animation frames.
     */
    explicit PowerUpView(std::shared_ptr<const logic::PowerUp> model, std::shared_ptr<sf::Texture> texture,
                         const std::array<sf::IntRect, 2>& animationFrames);

    /**
     * @brief Marks the view for removal when its power-up is collected.
     */
    void onNotify(const logic::Subject& source, logic::EventType event) override;

    /**
     * @brief Updates the animation and draws the power-up at its model position.
     */
    void draw(sf::RenderWindow& window, const Camera& camera) override;

    /**
     * @brief Returns the two spritesheet frames corresponding to a power-up type.
     */
    static std::array<sf::IntRect, 2> getPowerUpAnimation(PowerUpSpriteVariant variant);

private:
    /// The power-up model whose position and state are displayed by this view.
    std::shared_ptr<const logic::PowerUp> model;

    /// SFML sprite used to render the power-up.
    sf::Sprite sprite;

    /// Spritesheet containing the power-up graphics.
    std::shared_ptr<sf::Texture> texture;

    /// The two spritesheet regions used for the power-up's animation.
    std::array<sf::IntRect, 2> animationFrames;

    /// Accumulated time since the current animation frame was selected.
    float animationTimer = 0.0f;

    /// Index of the currently displayed animation frame.
    std::size_t currentFrame = 0;

    /// Time each animation frame remains visible, in seconds.
    static constexpr float kAnimationFrameDuration = 0.5f;
};

} // namespace bomberman::representation