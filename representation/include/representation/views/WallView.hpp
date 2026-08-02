#pragma once

#include "logic/entities/Wall.hpp"
#include "representation/views/EntityView.hpp"
#include <memory>
#include <array>

namespace bomberman::representation {

enum class WallSpriteVariant { Destructible, Indestructible };

struct WallAnimationSet {
    std::array<sf::IntRect, 1> idle;
    std::array<sf::IntRect, 6> breaking;
};

WallAnimationSet makeWallAnimationSet(WallSpriteVariant variant);

/// TODO: pick the destructible or indestructible sprite based on
/// model_->isDestructible() once in the constructor (Walls are static, no
/// animation needed besides possibly a short "crumble" on BlockDestroyed).
class WallView : public EntityView {
public:
    WallView(std::shared_ptr<const bomberman::logic::Wall> model,
             std::shared_ptr<sf::Texture> texture,
             WallAnimationSet animationSet);

    void onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) override; // TODO
    void draw(sf::RenderWindow& window, const Camera& camera) override; // TODO

private:
    std::shared_ptr<const bomberman::logic::Wall> model;
    sf::Sprite sprite;
    std::shared_ptr<sf::Texture> texture;
    WallAnimationSet animationSet;

    bool breaking{false};
    float breakTimer{0.f};
    static constexpr float kBreakDuration{0.25f};
};

} // namespace bomberman::representation
