#include "representation/ConcreteFactory.hpp"
#include "logic/entities/Bomb.hpp"
#include "logic/entities/BombPowerUp.hpp"
#include "logic/entities/Bot.hpp"
#include "logic/entities/FirePowerUp.hpp"
#include "logic/entities/Player.hpp"
#include "logic/entities/SkatesPowerUp.hpp"
#include "logic/entities/Wall.hpp"
#include "representation/views/BombView.hpp"
#include "representation/views/CharacterView.hpp"
#include "representation/views/PowerUpView.hpp"
#include "representation/views/WallView.hpp"

namespace bomberman::representation {

using namespace bomberman::logic;

std::shared_ptr<Character> ConcreteFactory::createCharacter(Vector2 position, bool isPlayer) {
    // TODO: pick a fixed size (e.g. {0.08f, 0.08f}), construct a Player or a
    // Bot depending on isPlayer, construct a matching CharacterView,
    // model->attach(view), store the view in views_, return the model. See
    // the ConcreteFactory.hpp doc comment for the full 5-step recipe.
    (void)position;
    (void)isPlayer;
    return nullptr;
}

std::shared_ptr<Bomb> ConcreteFactory::createBomb(Vector2 position, Character& owner) {
    // TODO: same recipe, using owner.getBombRadius() for the Bomb's radius
    // and a BombView.
    (void)position;
    (void)owner;
    return nullptr;
}

std::shared_ptr<Wall> ConcreteFactory::createWall(Vector2 position, bool destructible) {
    // TODO: same recipe, using a WallView.
    (void)position;
    (void)destructible;
    return nullptr;
}

std::shared_ptr<PowerUp> ConcreteFactory::createPowerUp(Vector2 position, PowerUpType type) {
    // TODO: switch (type) to construct a Fire/Bomb/SkatesPowerUp, then the
    // same recipe using a PowerUpView.
    (void)position;
    (void)type;
    return nullptr;
}

} // namespace bomberman::representation
