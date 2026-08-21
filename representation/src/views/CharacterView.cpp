#include "representation/views/CharacterView.hpp"
#include <array>

namespace bomberman::representation {

namespace {
constexpr int kFrameWidth = 16;
constexpr int kFrameHeight = 24;
constexpr int kWinFrameHeight = 40;
constexpr float kFrameSeconds = 0.12f;

using Frames4 = std::array<sf::IntRect, 4>;
using WalkSet = std::array<Frames4, 4>; // [direction][frame]
using Frames12 = std::array<sf::IntRect, 12>;

sf::IntRect rect(int left, int top) { return {left, top, kFrameWidth, kFrameHeight}; }
sf::IntRect winRect(int left, int top) { return {left, top, kFrameWidth, kWinFrameHeight}; }

std::size_t dirIndex(logic::Direction direction) {
    switch (direction) {
    case logic::Direction::Up:
        return 0;
    case logic::Direction::Down:
        return 1;
    case logic::Direction::Left:
        return 2;
    case logic::Direction::Right:
        return 3;
    case logic::Direction::None:
        return 1; // Default to Down
    }
    return 1; // Default to Down
}

CharacterAnimationSet makeSet(const Frames4& idle, const WalkSet& walk, const Frames12& win = {},
                              const Frames12& dead = {}) {
    return {idle, walk, win, dead};
}
} // namespace

CharacterAnimationSet makeCharacterAnimationSet(CharacterSpriteVariant variant) {
    switch (variant) {
    case CharacterSpriteVariant::Player:
        return makeSet(Frames4{rect(20, 97), rect(20, 47), rect(20, 122), rect(20, 72)},
                       WalkSet{Frames4{rect(3, 97), rect(20, 97), rect(37, 97), rect(20, 97)},
                               Frames4{rect(3, 47), rect(20, 47), rect(37, 47), rect(20, 47)},
                               Frames4{rect(3, 122), rect(20, 122), rect(37, 122), rect(20, 122)},
                               Frames4{rect(3, 72), rect(20, 72), rect(37, 72), rect(20, 72)}},
                       Frames12{winRect(3, 198), winRect(20, 198), winRect(37, 198), winRect(54, 198), winRect(71, 198),
                                winRect(88, 198), winRect(105, 198), winRect(88, 198), winRect(71, 198),
                                winRect(54, 198), winRect(37, 198), winRect(20, 198)},
                       Frames12{rect(3, 160), rect(20, 160), rect(37, 160), rect(54, 160), rect(71, 160), rect(88, 160),
                                rect(105, 160), rect(105, 160), rect(105, 160), rect(105, 160), rect(105, 160),
                                rect(105, 160)});

    case CharacterSpriteVariant::Bot1:
        return makeSet(Frames4{rect(213, 97), rect(213, 47), rect(213, 122), rect(213, 72)},
                       WalkSet{Frames4{rect(196, 97), rect(213, 97), rect(230, 97), rect(213, 97)},
                               Frames4{rect(196, 47), rect(213, 47), rect(230, 47), rect(213, 47)},
                               Frames4{rect(196, 122), rect(213, 122), rect(230, 122), rect(213, 122)},
                               Frames4{rect(196, 72), rect(213, 72), rect(230, 72), rect(213, 72)}},
                       Frames12{},
                       Frames12{rect(196, 160), rect(213, 160), rect(230, 160), rect(247, 160), rect(264, 160),
                                rect(281, 160), rect(298, 160), rect(298, 160), rect(298, 160), rect(298, 160),
                                rect(298, 160), rect(298, 160)});

    case CharacterSpriteVariant::Bot2:
        return makeSet(Frames4{rect(20, 355), rect(20, 305), rect(20, 380), rect(20, 330)},
                       WalkSet{Frames4{rect(3, 355), rect(20, 355), rect(37, 355), rect(20, 355)},
                               Frames4{rect(3, 305), rect(20, 305), rect(37, 305), rect(20, 305)},
                               Frames4{rect(3, 380), rect(20, 380), rect(37, 380), rect(20, 380)},
                               Frames4{rect(3, 330), rect(20, 330), rect(37, 330), rect(20, 330)}},
                       Frames12{},
                       Frames12{rect(3, 418), rect(20, 418), rect(37, 418), rect(54, 418), rect(71, 418), rect(88, 418),
                                rect(105, 418), rect(105, 418), rect(105, 418), rect(105, 418), rect(105, 418),
                                rect(105, 418)});

    case CharacterSpriteVariant::Bot3:
        return makeSet(Frames4{rect(213, 355), rect(213, 305), rect(213, 380), rect(213, 330)},
                       WalkSet{Frames4{rect(196, 355), rect(213, 355), rect(230, 355), rect(213, 355)},
                               Frames4{rect(196, 305), rect(213, 305), rect(230, 305), rect(213, 305)},
                               Frames4{rect(196, 380), rect(213, 380), rect(230, 380), rect(213, 380)},
                               Frames4{rect(196, 330), rect(213, 330), rect(230, 330), rect(213, 330)}},
                       Frames12{},
                       Frames12{rect(196, 418), rect(213, 418), rect(230, 418), rect(247, 418), rect(264, 418),
                                rect(281, 418), rect(298, 418), rect(298, 418), rect(298, 418), rect(298, 418),
                                rect(298, 418), rect(298, 418)});
    }

    return makeSet(Frames4{rect(0, 0), rect(0, 0), rect(0, 0), rect(0, 0)}, WalkSet{});
}

void CharacterView::setWon() {
    won = true;
    frameTimer = 0.f;
    frameIndex = 0;
}

void CharacterView::setDead() {
    dead = true;
    frameTimer = 0.f;
    frameIndex = 0;
}

void CharacterView::onNotify(const logic::Subject& source, const logic::EventType event) {
    (void)source;
    if (event == logic::EventType::Died) {
        setDead();
    }
    if (event == logic::EventType::PlayerWon) {
        setWon();
    }
}

CharacterView::CharacterView(std::shared_ptr<const logic::Character> model, std::shared_ptr<sf::Texture> texture,
                             const CharacterAnimationSet& animationSet)
    : model(std::move(model)), texture(std::move(texture)), animationSet(animationSet) {
    sprite.setTexture(*this->texture);
}

void CharacterView::update(float deltaTime) {
    if (!model)
        return;

    if (won) {
        frameTimer += deltaTime;
        while (frameTimer >= kFrameSeconds) {
            frameTimer -= kFrameSeconds;
            frameIndex = (frameIndex + 1) % animationSet.win.size();
        }
        return;
    }

    if (dead) {
        frameTimer += deltaTime;
        while (frameTimer >= kFrameSeconds) {
            frameTimer -= kFrameSeconds;
            if (frameIndex < animationSet.dead.size() - 1) {
                ++frameIndex;
            } else {
                markedForRemoval = true;
                break;
            }
        }
        return;
    }

    if (!model->isMoving()) {
        frameTimer = 0.f;
        frameIndex = 0;
        lastFacing = model->getFacing();
        return;
    }

    const auto facing = model->getFacing();
    if (facing != lastFacing) {
        lastFacing = facing;
        frameTimer = 0.f;
        frameIndex = 0;
    }

    frameTimer += deltaTime;
    while (frameTimer >= kFrameSeconds) {
        frameTimer -= kFrameSeconds;
        frameIndex = (frameIndex + 1) % animationSet.walk[dirIndex(facing)].size();
    }
}

void CharacterView::draw(sf::RenderWindow& window, const Camera& camera) {
    if (!model || markedForRemoval)
        return;

    const auto screenPos = camera.worldToScreen(model->getPosition());
    const auto screenSize = camera.worldSizeToScreen(model->getSize());
    const auto facing = model->getFacing();
    const auto facingIdx = dirIndex(facing);

    const sf::IntRect frame = won                 ? animationSet.win[frameIndex]
                              : dead              ? animationSet.dead[frameIndex]
                              : model->isMoving() ? animationSet.walk[facingIdx][frameIndex]
                                                  : animationSet.idle[facingIdx];

    sprite.setTextureRect(frame);
    if (!won) {
        sprite.setOrigin(frame.width * 0.5f, frame.height * 0.5f);
    } else {
        sprite.setOrigin(frame.width * 0.5f, kFrameHeight);
    }
    sprite.setPosition(screenPos.x, screenPos.y);
    sprite.setScale(screenSize.x / static_cast<float>(kFrameWidth), screenSize.y / static_cast<float>(kFrameHeight));

    window.draw(sprite);
}

} // namespace bomberman::representation
