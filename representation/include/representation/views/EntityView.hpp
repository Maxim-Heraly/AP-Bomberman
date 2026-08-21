#pragma once

#include "logic/patterns/Observer.hpp"
#include "representation/Camera.hpp"
#include <SFML/Graphics.hpp>

namespace bomberman::representation {

/**
 * @brief Abstract View and Observer for an EntityModel.
 *
 * Concrete subclasses (CharacterView, BombView, WallView, PowerUpView) are
 * created by ConcreteFactory alongside their Model, and attached to it as an
 * Observer - see ConcreteFactory.hpp.
 */
class EntityView : public logic::Observer {
public:
    /**
     * @brief Virtual destructor for the EntityView base class.
     *
     * Allows concrete View objects to be destroyed correctly through a
     * pointer to EntityView.
     */
    ~EntityView() override = default;

    /**
     * @brief Updates the View's animation or visual state.
     *
     * Concrete Views can override this to advance animations or perform
     * other time-dependent visual updates. The base implementation does
     * nothing.
     *
     * @param deltaTime Time elapsed since the previous update, in seconds.
     */
    virtual void update(const float deltaTime) { (void)deltaTime; }

    /**
     * @brief Draws the entity represented by this View.
     *
     * Concrete Views implement this function to render their entity using
     * the given window and camera.
     *
     * @param window SFML window to draw the entity to.
     * @param camera Camera used to convert world coordinates to screen
     *               coordinates.
     */
    virtual void draw(sf::RenderWindow& window, const Camera& camera) = 0;

    /**
     * @brief Checks whether this View should be removed.
     *
     * A View can mark itself for removal when its corresponding entity is
     * no longer alive or when its visual effect has finished.
     *
     * @return true if the View is marked for removal, false otherwise.
     */
    [[nodiscard]] bool isMarkedForRemoval() const { return markedForRemoval; }

    /**
     * @brief Returns the drawing layer of this View.
     *
     * Views with a higher draw layer can be rendered after Views with a
     * lower layer, allowing certain objects to appear in front of others.
     * The default layer is 0.
     *
     * @return The draw layer used when ordering Views for rendering.
     */
    [[nodiscard]] virtual int getDrawLayer() const { return 0; }

protected:
    /**
     * @brief Indicates whether this View should be removed from the
     * representation.
     *
     * Concrete Views can set this to true when they are finished and should
     * no longer be updated or drawn.
     */
    bool markedForRemoval{false};
};

} // namespace bomberman::representation