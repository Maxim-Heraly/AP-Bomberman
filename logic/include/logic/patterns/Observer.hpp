#pragma once

namespace bomberman::logic {

class Subject;

/**
 * @brief The type of event a Subject can notify its Observers about.
 *
 * This single enum is shared by every part of the game that participates in
 * the Observer pattern (Views updating their sprites/animations, and Score
 * updating the player's score). Extend this list as your design grows (e.g.
 * splitting BlockDestroyed per powerup type, or adding BotDied vs PlayerDied).
 */
enum class EventType {
    Tick,               // Emitted every update step, mainly so Views stay in sync each frame.
    Moved,               // Entity changed position and/or facing direction.
    BombPlaced,           // A Character placed a new Bomb.
    BombExploded,          // A Bomb finished its countdown and exploded.
    BlockDestroyed,        // A destructible Wall was destroyed by an explosion.
    PowerUpCollected,       // A Character walked over and collected a PowerUp.
    Died,                    // An Entity (Character) was caught in an explosion.
    PlayerWon,                // The human Player is the last one standing.
    PlayerLost,                 // The human Player died.
    PlayerCollectedPowerUp,     // scoring events
    PlayerDestroyedBlock,       // ||
    EnemyKilled                 // ||
};

/**
 * @brief Abstract Observer interface (Observer design pattern, see section 3.1.1).
 *
 * TODO: Both EntityView (representation, see representation/views/EntityView.hpp)
 * and Score (logic/Score.hpp) should inherit from this class so they can
 * subscribe to updates from an EntityModel (see Subject.hpp) without
 * EntityModel ever needing to know about them directly.
 */
class Observer {
public:
    virtual ~Observer() = default;

    /**
     * @brief Called by a Subject when it wants to notify this Observer.
     * @param source The Subject that triggered the notification (e.g. an EntityModel).
     * @param event The type of event that occurred.
     *
     * TODO (Score): react to BlockDestroyed / PowerUpCollected / Died /
     *   PlayerWon / PlayerLost by applying the appropriate score modifier.
     * TODO (EntityView subclasses): react to Moved / Died / BombPlaced /
     *   BombExploded by switching the current animation/sprite.
     */
    virtual void onNotify(const Subject& source, EventType event) = 0;
};

} // namespace bomberman::logic
