#pragma once

#include "logic/patterns/Observer.hpp"
#include "representation/Camera.hpp"
#include <SFML/Graphics.hpp>

namespace bomberman::representation {

/**
 * @brief Abstract View and Observer for an EntityModel.
 * Concrete subclasses (CharacterView, BombView, WallView, PowerUpView) are
 * created by ConcreteFactory alongside their Model, and attached to it as
 * an Observer - see ConcreteFactory.hpp.
 */
class EntityView : public logic::Observer {
public:
    ~EntityView() override = default;

    virtual void update(float deltaTime) {(void)deltaTime;}
    virtual void draw(sf::RenderWindow& window, const Camera& camera) = 0;

    bool isMarkedForRemoval() const { return markedForRemoval; }

protected:
    bool markedForRemoval{false};
};

} // namespace bomberman::representation
