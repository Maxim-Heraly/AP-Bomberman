// Minimal, dependency-free test harness

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

#include "logic/Score.hpp"
#include "logic/entities/BombPowerUp.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/Character.hpp"
#include "logic/entities/EntityModel.hpp"
#include "logic/entities/FirePowerUp.hpp"
#include "logic/entities/SkatesPowerUp.hpp"
#include "logic/entities/Wall.hpp"
#include "logic/patterns/Observer.hpp"
#include "logic/patterns/Subject.hpp"
#include "logic/utils/Direction.hpp"
#include "logic/utils/Vector2.hpp"

#define ASSERT(condition, message)                                                   \
    if (!(condition)) {                                                              \
        std::cerr << "FAILED: " << message << " (" << __FILE__ << ":" << __LINE__ << ")\n"; \
        std::exit(1);                                                                \
    }

using namespace bomberman::logic;

namespace {

// EntityModel is abstract (pure-virtual update()), so intersects() and the
// other EntityModel-level behaviour need a trivial concrete stand-in to
// instantiate.
struct TestEntity : EntityModel {
    TestEntity(Vector2 pos, Vector2 size) : EntityModel(pos, size) {}
    void update(float /*deltaTime*/) override {}
};

// Generic spy Observer: records every event it's notified about, so tests
// can assert both "was notified" and "with which EventType", without
// needing a real View or Score.
struct RecordingObserver : Observer {
    std::vector<EventType> events;
    void onNotify(const Subject& /*source*/, EventType event) override {
        events.push_back(event);
    }
};

} // namespace

// ---------------------------------------------------------------------
// Vector2 / Direction
// ---------------------------------------------------------------------

void testVector2Operators() {
    Vector2 a{1.f, 2.f};
    Vector2 b{3.f, 4.f};

    const Vector2 sum = a + b;
    ASSERT(sum.x == 4.f && sum.y == 6.f, "operator+ should add components");

    const Vector2 diff = b - a;
    ASSERT(diff.x == 2.f && diff.y == 2.f, "operator- should subtract components");

    const Vector2 scaled = a * 2.f;
    ASSERT(scaled.x == 2.f && scaled.y == 4.f, "operator* should scale both components");

    Vector2 accumulator{0.f, 0.f};
    accumulator += a;
    ASSERT(accumulator.x == 1.f && accumulator.y == 2.f, "operator+= should accumulate components");

    accumulator -= b;
    ASSERT(accumulator.x == -2.f && accumulator.y == -2.f, "operator-= should subtract components in place");
}

void testDirectionToVector() {
    ASSERT(directionToVector(Direction::Up).y < 0.f, "Up should move along -y");
    ASSERT(directionToVector(Direction::Down).y > 0.f, "Down should move along +y");
    ASSERT(directionToVector(Direction::Left).x < 0.f, "Left should move along -x");
    ASSERT(directionToVector(Direction::Right).x > 0.f, "Right should move along +x");

    const Vector2 none = directionToVector(Direction::None);
    ASSERT(none.x == 0.f && none.y == 0.f, "None should be a zero vector");
}

// ---------------------------------------------------------------------
// EntityModel
// ---------------------------------------------------------------------

void testEntityModelIntersects() {
    TestEntity a({0.f, 0.f}, {0.2f, 0.2f});
    TestEntity overlapping({0.1f, 0.1f}, {0.2f, 0.2f});
    TestEntity farAway({5.f, 5.f}, {0.2f, 0.2f});

    ASSERT(a.intersects(overlapping), "Overlapping entities should intersect");
    ASSERT(overlapping.intersects(a), "intersects() should be symmetric");
    ASSERT(!a.intersects(farAway), "Far-apart entities should not intersect");

    // (size.x + other.size.x) * 0.5 == 0.2, exactly the gap here, and the
    // check uses strict '<' - so boxes exactly edge-to-edge don't count as
    // overlapping.
    TestEntity touching({0.2f, 0.f}, {0.2f, 0.2f});
    ASSERT(!a.intersects(touching), "Entities exactly touching at the edge should not count as intersecting");
}

// ---------------------------------------------------------------------
// Wall
// ---------------------------------------------------------------------

void testWallDestroy() {
    Wall wall({0.f, 0.f}, {0.1f, 0.1f}, /*destructible=*/true);
    ASSERT(wall.isAlive(), "A freshly created wall should be alive");
    ASSERT(wall.isDestructible(), "This wall was created as destructible");

    wall.destroy();
    ASSERT(!wall.isAlive(), "destroy() should mark the wall as dead");
}

void testWallNotifiesObserversOnDestroy() {
    auto wall = std::make_shared<Wall>(Vector2{0.f, 0.f}, Vector2{0.1f, 0.1f}, true);
    auto observer = std::make_shared<RecordingObserver>();
    wall->attach(observer);

    wall->destroy();

    ASSERT(observer->events.size() == 1, "Destroying a wall should notify observers exactly once");
    ASSERT(observer->events[0] == EventType::BlockDestroyed, "Wall::destroy() should notify BlockDestroyed");
}

// ---------------------------------------------------------------------
// Bomb
// ---------------------------------------------------------------------

void testBombFuseCountsDown() {
    const std::weak_ptr<Character> noOwner;
    Bomb bomb({0.f, 0.f}, {0.1f, 0.1f}, noOwner, /*radius=*/1);
    ASSERT(!bomb.hasExploded(), "A freshly placed bomb should not have exploded yet");

    bomb.update(1.f);
    ASSERT(!bomb.hasExploded(), "Bomb should still be ticking after 1 of 2 seconds");

    bomb.update(.5f);
    ASSERT(!bomb.hasExploded(), "Bomb should still be ticking after 1.5 of 2 seconds");

    bomb.update(1.f); // total elapsed: 2.5s, past the 2s fuse
    ASSERT(bomb.hasExploded(), "Bomb should have exploded once its fuse ran out");
    ASSERT(!bomb.isAlive(), "An exploded bomb should be marked dead");
}

void testBombDetonateEarly() {
    const std::weak_ptr<Character> noOwner;
    Bomb bomb({0.f, 0.f}, {0.1f, 0.1f}, noOwner, /*radius=*/2);

    bomb.detonateEarly();
    bomb.update(0.f);

    ASSERT(bomb.hasExploded(), "detonateEarly() should force the bomb to explode on its next update()");
}

void testBombOwnerPassThroughToggle() {
    const std::weak_ptr<Character> noOwner;
    Bomb bomb({0.f, 0.f}, {0.1f, 0.1f}, noOwner, 1);

    ASSERT(bomb.canOwnerPassThrough(), "A freshly placed bomb should let its owner walk through it");
    bomb.disableOwnerPassThrough();
    ASSERT(!bomb.canOwnerPassThrough(), "disableOwnerPassThrough() should turn pass-through off");
}

void testBombNotifiesOwnerOnExplosion() {
    auto owner = std::make_shared<Character>(Vector2{0.f, 0.f}, Vector2{0.1f, 0.1f});
    ASSERT(owner->tryPlaceBomb(), "Owner should be able to place its first bomb");
    ASSERT(!owner->canPlaceBomb(), "Owner should have used its only bomb slot");

    Bomb bomb({0.f, 0.f}, {0.1f, 0.1f}, owner, 1);
    bomb.update(3.5f); // past the 3s fuse

    ASSERT(bomb.hasExploded(), "Bomb should have exploded");
    ASSERT(owner->canPlaceBomb(), "Exploding should free the owner's bomb slot via Character::onBombExploded()");
}

// ---------------------------------------------------------------------
// Character
// ---------------------------------------------------------------------

void testCharacterBombPlacementLimits() {
    Character character({0.f, 0.f}, {0.1f, 0.1f});
    ASSERT(character.canPlaceBomb(), "A fresh character (maxBombs == 1) should be able to place a bomb");

    ASSERT(character.tryPlaceBomb(), "tryPlaceBomb() should succeed while under the limit");
    ASSERT(!character.canPlaceBomb(), "The character should be out of bomb slots after placing its only bomb");
    ASSERT(!character.tryPlaceBomb(), "tryPlaceBomb() should fail once at the limit");

    character.onBombExploded();
    ASSERT(character.canPlaceBomb(), "Once a placed bomb explodes, the slot should free up again");
}

void testCharacterDie() {
    auto character = std::make_shared<Character>(Vector2{0.f, 0.f}, Vector2{0.1f, 0.1f});
    auto observer = std::make_shared<RecordingObserver>();
    character->attach(observer);

    ASSERT(character->isAlive(), "A fresh character should be alive");

    character->die();
    ASSERT(!character->isAlive(), "die() should mark the character dead");
    ASSERT(observer->events.size() == 1 && observer->events[0] == EventType::Died,
           "die() should notify Died exactly once");

    character->die(); // already dead - should be a no-op
    ASSERT(observer->events.size() == 1, "Calling die() again on a dead character should not re-notify Died");
}

void testCharacterMovementAndRevert() {
    Character character({0.f, 0.f}, {0.1f, 0.1f});
    character.setMovementInput(Direction::Right);
    character.update(1.0f); // default speed is 0.5 units/sec

    ASSERT(character.getPosition().x > 0.f, "Character should have moved right after update()");
    ASSERT(character.getFacing() == Direction::Right, "Character should now be facing the direction it moved in");

    character.revertToPreviousPosition();
    ASSERT(std::abs(character.getPosition().x) < 1e-5f, "revertToPreviousPosition() should undo the last movement");
}

// ---------------------------------------------------------------------
// PowerUps
// ---------------------------------------------------------------------

void testFirePowerUpIncreasesBombRadius() {
    Character character({0.f, 0.f}, {0.1f, 0.1f});
    const int initialRadius = character.getBombRadius();

    FirePowerUp powerUp({0.f, 0.f}, {0.1f, 0.1f});
    powerUp.applyEffect(character);

    ASSERT(character.getBombRadius() == initialRadius + 1, "FirePowerUp should increase bomb radius by 1");
    ASSERT(!powerUp.isAlive(), "Applying a power-up's effect should consume/mark it dead");
}

void testBombPowerUpIncreasesMaxBombs() {
    Character character({0.f, 0.f}, {0.1f, 0.1f});
    const int initialMax = character.getMaxBombs();

    BombPowerUp powerUp({0.f, 0.f}, {0.1f, 0.1f});
    powerUp.applyEffect(character);

    ASSERT(character.getMaxBombs() == initialMax + 1, "BombPowerUp should increase max simultaneous bombs by 1");
}

void testSkatesPowerUpIncreasesSpeed() {
    Character character({0.f, 0.f}, {0.1f, 0.1f});
    const float initialSpeed = character.getSpeed();

    SkatesPowerUp powerUp({0.f, 0.f}, {0.1f, 0.1f});
    powerUp.applyEffect(character);

    ASSERT(std::abs(character.getSpeed() - (initialSpeed + 0.1f)) < 1e-5f,
           "SkatesPowerUp should increase speed by 0.1");
}

// ---------------------------------------------------------------------
// Score
// ---------------------------------------------------------------------

void testScoring() {
    auto score = Score::getInstance();
    score->setCurrentScore(0);

    score->addBlockDestroyed();
    ASSERT(score->getCurrentScore() == 10, "Destroying a block should add 10 points");

    score->addPowerUpCollected();
    ASSERT(score->getCurrentScore() == 35, "Collecting a power-up should add 25 points");

    score->addEnemyKilled();
    ASSERT(score->getCurrentScore() == 135, "Killing an enemy should add 100 points");

    score->addPlayerWon();
    ASSERT(score->getCurrentScore() == 635, "Winning should add 500 points");

    score->addPlayerLost();
    ASSERT(score->getCurrentScore() == 535, "Losing should subtract 100 points");

    score->setCurrentScore(0); // leave the singleton clean for later tests
}

void testScoreOnNotifyDispatchesEvents() {
    auto score = Score::getInstance();
    score->setCurrentScore(0);
    Subject subject;

    score->onNotify(subject, EventType::PlayerDestroyedBlock);
    score->onNotify(subject, EventType::PlayerCollectedPowerUp);
    score->onNotify(subject, EventType::EnemyKilled);
    ASSERT(score->getCurrentScore() == 135, "onNotify should route each scoring EventType to its handler");

    score->onNotify(subject, EventType::Moved); // not a scoring event
    ASSERT(score->getCurrentScore() == 135, "Non-scoring events like Moved should leave the score unchanged");

    score->setCurrentScore(0);
}

void testScoreHighScorePersistence() {
    auto score = Score::getInstance();
    const std::string path = "test_highscores_tmp.txt";
    std::remove(path.c_str());

    score->loadHighScores(path); // file doesn't exist yet
    ASSERT(score->getTopFive().empty(),
           "Loading a missing high-score file should not throw and should yield an empty list");

    score->setCurrentScore(777);
    score->saveHighScores(path);
    score->loadHighScores(path);

    ASSERT(score->getTopFive().size() == 1, "After saving once, the high-score file should round-trip one entry");
    ASSERT(score->getTopFive().front().score == 777, "The saved score should match what was written");

    std::remove(path.c_str());
    score->setCurrentScore(0);
}

int main() {
    testVector2Operators();
    testDirectionToVector();

    testEntityModelIntersects();

    testWallDestroy();
    testWallNotifiesObserversOnDestroy();

    testBombFuseCountsDown();
    testBombDetonateEarly();
    testBombOwnerPassThroughToggle();
    testBombNotifiesOwnerOnExplosion();

    testCharacterBombPlacementLimits();
    testCharacterDie();
    testCharacterMovementAndRevert();

    testFirePowerUpIncreasesBombRadius();
    testBombPowerUpIncreasesMaxBombs();
    testSkatesPowerUpIncreasesSpeed();

    testScoring();
    testScoreOnNotifyDispatchesEvents();
    testScoreHighScorePersistence();

    std::cout << "All tests passed.\n";
    return 0;
}