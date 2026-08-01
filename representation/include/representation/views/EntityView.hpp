#pragma once

#include "logic/patterns/Observer.hpp"
#include "representation/Camera.hpp"
#include <SFML/Graphics.hpp>

namespace bomberman::representation {

/**
 * @brief Abstract View (MVC, section 3.1.1) and Observer for an EntityModel.
 * Concrete subclasses (CharacterView, BombView, WallView, PowerUpView) are
 * created by ConcreteFactory alongside their Model, and attached to it as
 * an Observer - see ConcreteFactory.hpp.
 *
 * TODO: give each concrete subclass a sf::Sprite (+ sf::Texture) member and
 * load its texture from the shared spritesheet - see
 * assets/spritesheets/README.md for where the file should live and how to
 * reference it (an ASSET_DIR macro is defined for you, see
 * representation/CMakeLists.txt).
 */
class EntityView : public bomberman::logic::Observer {
public:
    ~EntityView() override = default;

    /// TODO: draw this entity's current sprite, projected through camera,
    /// onto window. Called once per frame by PlayState::render().
    virtual void update(float deltaTime) {(void)deltaTime;}
    virtual void draw(sf::RenderWindow& window, const Camera& camera) = 0;

    /// Set (by onNotify(), on EventType::Died, once any death animation has
    /// finished) so ConcreteFactory/PlayState know when a View can be
    /// dropped from the views_ list.
    bool isMarkedForRemoval() const { return markedForRemoval; }

protected:
    bool markedForRemoval{false};
};

} // namespace bomberman::representation
