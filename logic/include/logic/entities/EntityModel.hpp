#pragma once

#include "logic/patterns/Subject.hpp"
#include "logic/utils/Vector2.hpp"

namespace bomberman::logic {

/**
 * @brief Base class ("Model" in MVC, see section 3.1.1) for every object
 * that lives in the World: Character, Bomb, Wall and PowerUp all derive
 * from this.
 *
 * It is a Subject so its View (attached via the ConcreteFactory) and Score
 * can be notified whenever something about it changes.
 */
class EntityModel : public Subject {
public:
    EntityModel(Vector2 position, Vector2 size) : position(position), size(size) {}
    ~EntityModel() override = default;

    [[nodiscard]] const Vector2& getPosition() const { return position; }
    [[nodiscard]] const Vector2& getSize() const { return size; }

    [[nodiscard]] bool isAlive() const { return alive; }

    /// TODO: implement in each subclass - advance this entity's state by one
    /// tick. Call notify(EventType::...) whenever something happens that
    /// Score/Views need to know about (e.g. notify(EventType::Moved) after
    /// changing position_).
    virtual void update(float deltaTime) = 0;

    /// TODO: use this in World::handleCollisions() - basic axis-aligned
    /// bounding-box overlap test, centered on position_ with size_ as full
    /// width/height. Do NOT use sf::FloatRect here: this class must stay
    /// completely SFML-free (see section 3.1, "Code Design").
    [[nodiscard]] bool intersects(const EntityModel& other) const;

protected:
    Vector2 position;
    Vector2 size;
    bool alive{true};

    /// TODO: call this (together with notify(EventType::Died) where
    /// appropriate) from subclasses when this entity should be removed by
    /// World on its next update.
    void markDead() { alive = false; }
};

} // namespace bomberman::logic
