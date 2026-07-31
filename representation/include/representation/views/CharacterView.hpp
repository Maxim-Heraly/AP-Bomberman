#pragma once

#include "logic/entities/Character.hpp"
#include "representation/views/EntityView.hpp"
#include <memory>

namespace bomberman::representation {

/**
 * @brief TODO:
 *   - Hold a sf::Sprite (+ a shared/cached sf::Texture - load it ONCE from
 *     the spritesheet and reuse it across every CharacterView instance,
 *     rather than reloading per-instance).
 *   - onNotify(): switch animation frame based on the received EventType
 *     (Moved -> advance the walk-cycle frame for model_->getFacing(), Died
 *     -> switch to and play the death animation).
 *   - draw(): position the sprite via camera.worldToScreen(model_->getPosition()).
 */
class CharacterView : public EntityView {
public:
    explicit CharacterView(std::shared_ptr<const bomberman::logic::Character> model)
        : model(std::move(model)) {}

    void onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) override; // TODO
    void draw(sf::RenderWindow& window, const Camera& camera) override; // TODO

private:
    std::shared_ptr<const bomberman::logic::Character> model;
    // TODO: sf::Sprite sprite_; animation frame index + timer, one walk-cycle per Direction, a death animation, ...
};

} // namespace bomberman::representation
