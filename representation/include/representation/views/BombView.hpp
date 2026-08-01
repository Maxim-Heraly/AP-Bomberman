#pragma once

#include "logic/entities/Bomb.hpp"
#include "representation/views/EntityView.hpp"
#include <memory>
#include <array>

namespace bomberman::representation {

struct BombAnimationSet {
    std::array<sf::IntRect, 4> bomb;
    std::array<sf::IntRect, 9> center;
    std::array<sf::IntRect, 9> endUp;
    std::array<sf::IntRect, 9> endDown;
    std::array<sf::IntRect, 9> endLeft;
    std::array<sf::IntRect, 9> endRight;
    std::array<sf::IntRect, 9> bodyVertical;
    std::array<sf::IntRect, 9> bodyHorizontal;
};

BombAnimationSet makeBombAnimationSet();

/**
 * @brief TODO:
 *   - onNotify(): play the "ticking" animation as the fuse counts down
 *     (section 2.2, "An animation should play when each bomb ticks down"),
 *     then switch to the explosion animation on BombExploded.
 *   - draw(): position via camera.worldToScreen(model_->getPosition()).
 */
class BombView : public EntityView {
public:
    BombView(std::shared_ptr<const bomberman::logic::Bomb> model,
             std::shared_ptr<sf::Texture> texture,
             BombAnimationSet animations);

    void onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) override; // TODO
    void draw(sf::RenderWindow& window, const Camera& camera) override;
    void update(float deltaTime) override;

private:
    enum class Phase { Fuse, Explosion, Done };

    void drawFrame(sf::RenderWindow& window, const Camera& camera,
                   const sf::IntRect& frame, const bomberman::logic::Vector2& worldPos) const;

    std::shared_ptr<const bomberman::logic::Bomb> model;
    std::shared_ptr<sf::Texture> texture;
    BombAnimationSet animations;
    mutable sf::Sprite sprite;

    Phase phase{Phase::Fuse};

    float fuseFrameTimer{0.f};
    std::size_t fuseFrameIndex{0};

    float explosionFrameTimer{0.f};
    std::size_t explosionFrameIndex{0};
};

} // namespace bomberman::representation
