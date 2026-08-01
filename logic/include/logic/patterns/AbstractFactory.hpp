#pragma once

#include "logic/utils/Vector2.hpp"
#include <memory>

namespace bomberman::logic {

class EntityModel;
class Character;
class Bomb;
class Wall;
class PowerUp;

enum class PowerUpType { Fire, ExtraBomb, Skates };

/**
 * @brief Abstract Factory (design pattern, see section 3.1.1) interface,
 * defined in the logic library so World can create new entities without
 * knowing anything about SFML. The representation library provides a
 * ConcreteFactory implementing these methods: for every Model it creates,
 * it should also construct the matching View and attach() it as an Observer
 * (see Subject.hpp) before returning the Model to World.
 *
 * TODO: adjust these signatures as your Character/Bomb/etc. constructors
 * need more parameters (e.g. an enum to distinguish Player vs Bot, or a
 * concrete Bot "personality" for the smarter-bots bonus in section 2.3).
 */
class AbstractFactory {
public:
    virtual ~AbstractFactory() = default;

    virtual std::shared_ptr<Character> createCharacter(Vector2 position, bool isPlayer) = 0;
    virtual std::shared_ptr<Bomb> createBomb(Vector2 position, std::shared_ptr<Character> owner) = 0;
    virtual std::shared_ptr<Wall> createWall(Vector2 position, bool destructible) = 0;
    virtual std::shared_ptr<PowerUp> createPowerUp(Vector2 position, PowerUpType type) = 0;
};

} // namespace bomberman::logic
