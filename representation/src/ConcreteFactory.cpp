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

ConcreteFactory::ConcreteFactory() {
    characterTexture = std::make_shared<sf::Texture>();
    if (!characterTexture->loadFromFile(ASSET_DIR "/spritesheets/character_sprites.png")) {
        throw std::runtime_error("Failed to load character texture");
    }
    arenaTexture = std::make_shared<sf::Texture>();
    if (!arenaTexture->loadFromFile(ASSET_DIR "/spritesheets/battle_stage_sprites.png")) {
        throw std::runtime_error("Failed to load arena texture");
    }
}

std::shared_ptr<Character> ConcreteFactory::createCharacter(Vector2 position, bool isPlayer) {
    Vector2 characterSize{0.09f, 0.09f};
    std::shared_ptr<Character> model;
    if (isPlayer) {
        model = std::make_shared<Player>(position, characterSize);
    } else {
        model = std::make_shared<Bot>(position, characterSize);
    }

    const auto variant = isPlayer
        ? CharacterSpriteVariant::Player
        : static_cast<CharacterSpriteVariant>(1 + (nextBotVariant++ % 3));

    auto view = std::make_shared<CharacterView>(
        model,
        characterTexture,
        makeCharacterAnimationSet(variant));

    model->attach(view);
    views.push_back(view);
    return model;
}

std::shared_ptr<Bomb> ConcreteFactory::createBomb(Vector2 position, std::shared_ptr<Character> owner) {
    if (!owner) return nullptr;

    Vector2 bombSize{0.07f, 0.07f};
    auto model = std::make_shared<Bomb>(position, bombSize, owner, owner->getBombRadius());

    auto view = std::make_shared<BombView>(model, arenaTexture, makeBombAnimationSet());
    model->attach(view);
    views.push_back(view);

    return model;
}

std::shared_ptr<Wall> ConcreteFactory::createWall(Vector2 position, bool destructible) {
    Vector2 wallSize{0.1f, 0.1f};
    auto model = std::make_shared<Wall>(position, wallSize, destructible);
    auto view = std::make_shared<WallView>(model, arenaTexture, makeWallAnimationSet(destructible ? WallSpriteVariant::Destructible : WallSpriteVariant::Indestructible));
    model->attach(view);
    views.push_back(view);

    return model;
}

std::shared_ptr<PowerUp> ConcreteFactory::createPowerUp(Vector2 position, PowerUpType type) {
    // TODO: switch (type) to construct a Fire/Bomb/SkatesPowerUp, then the
    // same recipe using a PowerUpView.
    (void)position;
    (void)type;
    return nullptr;
}

} // namespace bomberman::representation
