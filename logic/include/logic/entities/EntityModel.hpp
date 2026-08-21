#pragma once

#include "logic/patterns/Subject.hpp"
#include "logic/utils/Vector2.hpp"

namespace bomberman::logic {

/**
 * @brief Base class ("Model" in MVC) for every object
 * that lives in the World: Character, Bomb, Wall and PowerUp all derive from
 * this.
 *
 * It is a Subject so its View (attached via the ConcreteFactory) and Score
 * can be notified whenever something about it changes.
 */
class EntityModel : public Subject {
public:
    /**
     * @brief Creates an entity with a position, visual size and collision hitbox.
     */
    EntityModel(Vector2 position, Vector2 size, Vector2 hitbox)
        : position(position), size(size), hitbox(hitbox) {}

    /**
     * @brief Creates an entity whose hitbox is the same as its size.
     */
    EntityModel(Vector2 position, Vector2 size)
        : EntityModel(position, size, size) {}

    /// Virtual destructor for safe destruction through an EntityModel pointer.
    ~EntityModel() override = default;

    /// @return The entity's current center position in world coordinates.
    [[nodiscard]] const Vector2& getPosition() const { return position; }

    /// @return The entity's full width and height used for its visual size.
    [[nodiscard]] const Vector2& getSize() const { return size; }

    /// @return The width and height of the area used for collision detection.
    [[nodiscard]] const Vector2& getHitbox() const { return hitbox; }

    /// @return True if the entity is still active in the World.
    [[nodiscard]] bool isAlive() const { return alive; }

    /**
     * @brief Advances the entity's state by one update tick.
     * @param deltaTime Time elapsed since the previous update, in seconds.
     */
    virtual void update(float deltaTime) = 0;

    /**
     * @brief Checks whether this entity's hitbox overlaps another entity's hitbox.
     * @param other The entity to test for collision with.
     * @return True when the two axis-aligned hitboxes overlap.
     */
    [[nodiscard]] bool intersects(const EntityModel& other) const;

protected:
    /// Center position of the entity in normalized world coordinates.
    Vector2 position;

    /// Full width and height of the entity, mainly used for its representation.
    Vector2 size;

    /// Full width and height of the collision area.
    Vector2 hitbox;

    /// Indicates whether the entity should remain active in the World.
    bool alive{true};

    /// Marks the entity as dead so the World can remove it during cleanup.
    void markDead() { alive = false; }
};

} // namespace bomberman::logic