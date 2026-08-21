#pragma once

#include "logic/entities/Bomb.hpp"
#include "representation/views/EntityView.hpp"
#include <array>
#include <memory>

namespace bomberman::representation {

/**
 * @brief Contains the sprite-sheet frames used to animate a Bomb and its explosion.
 *
 * Each explosion direction has separate body and end frames, while the center
 * and bomb arrays contain the frames for the explosion center and bomb fuse.
 */
struct BombAnimationSet {
    std::array<sf::IntRect, 4> bomb;           ///< Frames for the bomb's fuse animation.
    std::array<sf::IntRect, 9> center;         ///< Frames for the center of the explosion.
    std::array<sf::IntRect, 9> endUp;          ///< Frames for the upward end of the explosion.
    std::array<sf::IntRect, 9> endDown;        ///< Frames for the downward end of the explosion.
    std::array<sf::IntRect, 9> endLeft;        ///< Frames for the left end of the explosion.
    std::array<sf::IntRect, 9> endRight;       ///< Frames for the right end of the explosion.
    std::array<sf::IntRect, 9> bodyVertical;   ///< Frames for vertical explosion segments.
    std::array<sf::IntRect, 9> bodyHorizontal; ///< Frames for horizontal explosion segments.
};

/**
 * @brief Creates the complete set of bomb and explosion sprite-sheet regions.
 */
BombAnimationSet makeBombAnimationSet();

/**
 * @brief View responsible for rendering and animating a Bomb model.
 *
 * The view displays the bomb's fuse, then its directional explosion, and
 * marks itself for removal once the explosion animation has finished.
 */
class BombView : public EntityView {
public:
    /**
     * @brief Creates a view for a Bomb using the supplied texture and animation frames.
     */
    BombView(std::shared_ptr<const logic::Bomb> model, std::shared_ptr<sf::Texture> texture,
             const BombAnimationSet& animations);

    /**
     * @brief Reacts to bomb-related events from the model, notably the explosion event.
     */
    void onNotify(const logic::Subject& source, logic::EventType event) override;

    /**
     * @brief Draws the current bomb or explosion frame at the model's world position.
     */
    void draw(sf::RenderWindow& window, const Camera& camera) override;

    /**
     * @brief Advances the fuse or explosion animation according to elapsed time.
     */
    void update(float deltaTime) override;

private:
    /**
     * @brief Represents the current stage of the bomb's visual lifecycle.
     */
    enum class Phase { Fuse, Explosion, Done };

    /**
     * @brief Draws a single sprite-sheet frame at the given world position.
     */
    void drawFrame(sf::RenderWindow& window, const Camera& camera, const sf::IntRect& frame,
                   const logic::Vector2& worldPos) const;

    std::shared_ptr<const logic::Bomb> model; ///< Bomb model providing position, size, and blast information.
    std::shared_ptr<sf::Texture> texture;     ///< Shared texture containing all bomb and explosion sprites.
    BombAnimationSet animations;              ///< Sprite-sheet regions used by the animations.
    mutable sf::Sprite sprite;                ///< SFML sprite reused to render each animation frame.

    Phase phase{Phase::Fuse}; ///< Current visual phase: fuse, explosion, or finished.

    float fuseFrameTimer{0.f};     ///< Accumulated time used to advance the fuse animation.
    std::size_t fuseFrameIndex{0}; ///< Current frame of the bomb's fuse animation.

    float explosionFrameTimer{0.f};     ///< Accumulated time used to advance the explosion animation.
    std::size_t explosionFrameIndex{0}; ///< Current frame of the explosion animation.
};

} // namespace bomberman::representation