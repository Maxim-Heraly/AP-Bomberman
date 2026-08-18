#pragma once

#include "logic/entities/Character.hpp"
#include "representation/views/EntityView.hpp"
#include <memory>
#include <array>

namespace bomberman::representation {

enum class CharacterSpriteVariant {Player, Bot1, Bot2, Bot3};

struct CharacterAnimationSet {
    std::array<sf::IntRect, 4> idle;
    std::array<std::array<sf::IntRect, 4>, 4> walk; // [direction][frame]
    std::array<sf::IntRect, 12> win;
    std::array<sf::IntRect, 12> dead;
};

CharacterAnimationSet makeCharacterAnimationSet(CharacterSpriteVariant variant);


class CharacterView : public EntityView {
public:
    CharacterView(std::shared_ptr<const logic::Character> model,
                  std::shared_ptr<sf::Texture> texture,
                  CharacterAnimationSet animationSet);
    void update(float deltaTime) override;

    void onNotify(const logic::Subject& source, logic::EventType event) override;
    void draw(sf::RenderWindow& window, const Camera& camera) override;
    [[nodiscard]] int getDrawLayer() const override { return 1; }

    void setWon();
    void setDead();

private:
    std::shared_ptr<const logic::Character> model;
    sf::Sprite sprite;
    std::shared_ptr<sf::Texture> texture;
    CharacterAnimationSet animationSet;

    float frameTimer{0.f};
    std::size_t frameIndex{0};
    logic::Direction lastFacing{logic::Direction::Down};
    bool won{false};
    bool dead{false};
};

} // namespace bomberman::representation
