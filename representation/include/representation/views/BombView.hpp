#pragma once

#include "logic/entities/Bomb.hpp"
#include "representation/views/EntityView.hpp"
#include <memory>

namespace bomberman::representation {

/**
 * @brief TODO:
 *   - onNotify(): play the "ticking" animation as the fuse counts down
 *     (section 2.2, "An animation should play when each bomb ticks down"),
 *     then switch to the explosion animation on BombExploded.
 *   - draw(): position via camera.worldToScreen(model_->getPosition()).
 */
class BombView : public EntityView {
public:
    explicit BombView(std::shared_ptr<const bomberman::logic::Bomb> model) : model(std::move(model)) {}

    void onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) override; // TODO
    void draw(sf::RenderWindow& window, const Camera& camera) override; // TODO

private:
    std::shared_ptr<const bomberman::logic::Bomb> model;
    // TODO: sf::Sprite sprite_; ticking-animation frame/timer state...
};

} // namespace bomberman::representation
