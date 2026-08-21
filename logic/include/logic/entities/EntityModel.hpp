#pragma once

#include "logic/patterns/Subject.hpp"
#include "logic/utils/Vector2.hpp"

namespace bomberman::logic {

/**
 * @brief Base class ("Model" in MVC) for every object
 * that lives in the World: Character, Bomb, Wall and PowerUp all derive
 * from this.
 *
 * It is a Subject so its View (attached via the ConcreteFactory) and Score
 * can be notified whenever something about it changes.
 */
class EntityModel : public Subject {
public:
    EntityModel(Vector2 position, Vector2 size, Vector2 hitbox) : position(position), size(size), hitbox(hitbox) {}
    EntityModel(Vector2 position, Vector2 size) : EntityModel(position, size, size) {}
    ~EntityModel() override = default;

    [[nodiscard]] const Vector2& getPosition() const { return position; }
    [[nodiscard]] const Vector2& getSize() const { return size; }
    [[nodiscard]] const Vector2& getHitbox() const { return hitbox; }

    [[nodiscard]] bool isAlive() const { return alive; }

    /// Advance this entity's state by one
    /// tick. Call notify(EventType::...) whenever something happens that
    /// Score/Views need to know about (e.g. notify(EventType::Moved) after
    /// changing position).
    virtual void update(float deltaTime) = 0;

    /// Basic axis-aligned
    /// bounding-box overlap test, centered on position with size as full
    /// width/height.
    [[nodiscard]] bool intersects(const EntityModel& other) const;

protected:
    Vector2 position;
    Vector2 size;
    Vector2 hitbox;
    bool alive{true};

    void markDead() { alive = false; }
};

} // namespace bomberman::logic
