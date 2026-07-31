#pragma once

#include "logic/entities/Wall.hpp"
#include "representation/views/EntityView.hpp"
#include <memory>

namespace bomberman::representation {

/// TODO: pick the destructible or indestructible sprite based on
/// model_->isDestructible() once in the constructor (Walls are static, no
/// animation needed besides possibly a short "crumble" on BlockDestroyed).
class WallView : public EntityView {
public:
    explicit WallView(std::shared_ptr<const bomberman::logic::Wall> model) : model(std::move(model)) {}

    void onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) override; // TODO
    void draw(sf::RenderWindow& window, const Camera& camera) override; // TODO

private:
    std::shared_ptr<const bomberman::logic::Wall> model;
    // TODO: sf::Sprite sprite_;
};

} // namespace bomberman::representation
