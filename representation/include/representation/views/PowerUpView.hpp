#pragma once

#include "logic/entities/PowerUp.hpp"
#include "representation/views/EntityView.hpp"
#include <memory>

namespace bomberman::representation {

/// TODO: pick the correct icon based on the concrete PowerUp subtype (e.g.
/// dynamic_cast is explicitly discouraged, see section 3.2 - instead,
/// consider adding a `getType()`/enum accessor on PowerUp, or passing the
/// PowerUpType into this View's constructor from ConcreteFactory, which
/// already knows it).
class PowerUpView : public EntityView {
public:
    explicit PowerUpView(std::shared_ptr<const bomberman::logic::PowerUp> model) : model(std::move(model)) {}

    void onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) override; // TODO
    void draw(sf::RenderWindow& window, const Camera& camera) override; // TODO

private:
    std::shared_ptr<const bomberman::logic::PowerUp> model;
    // TODO: sf::Sprite sprite_;
};

} // namespace bomberman::representation
