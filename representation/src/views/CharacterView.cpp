#include "representation/views/CharacterView.hpp"
#include <array>

namespace bomberman::representation {

namespace {
    constexpr int kFrameWidth = 16;
    constexpr int kFrameHeight = 24;
    constexpr float kFrameSeconds = 0.12f;

    using Frames4 = std::array<sf::IntRect, 4>;
    using WalkSet = std::array<Frames4, 4>; // [direction][frame]

    sf::IntRect rect(int left, int top) {
        return {left, top, kFrameWidth, kFrameHeight};
    }

    std::size_t dirIndex(bomberman::logic::Direction direction) {
        switch (direction) {
            case bomberman::logic::Direction::Up: return 0;
            case bomberman::logic::Direction::Down: return 1;
            case bomberman::logic::Direction::Left: return 2;
            case bomberman::logic::Direction::Right: return 3;
            case bomberman::logic::Direction::None: return 1; // Default to Down
        }
        return 1; // Default to Down
    }

    CharacterAnimationSet makeSet(const Frames4& idle, const WalkSet& walk) {
        return {idle, walk};
    }
}

CharacterAnimationSet makeCharacterAnimationSet(CharacterSpriteVariant variant) {
    switch (variant) {
        case CharacterSpriteVariant::Player:
            return makeSet(
                Frames4{
                    rect(20,97),
                    rect(20, 47),
                    rect(20, 122),
                    rect(20, 72)
                },
                WalkSet{
                    Frames4{rect(3, 97), rect(20, 97), rect(37, 97), rect(20, 97)},
                    Frames4{rect(3, 47), rect(20, 47), rect(37, 47), rect(20, 47)},
                    Frames4{rect(3, 122), rect(20, 122), rect(37, 122), rect(20, 122)},
                    Frames4{rect(3, 72), rect(20, 72), rect(37, 72), rect(20, 72)}
                });

        case CharacterSpriteVariant::Bot1:
            return makeSet(
            Frames4{
                rect(20,97),
                rect(20, 47),
                rect(20, 122),
                rect(20, 72)
            },
            WalkSet{
                Frames4{rect(3, 97), rect(20, 97), rect(37, 97), rect(20, 97)},
                Frames4{rect(3, 47), rect(20, 47), rect(37, 47), rect(20, 47)},
                Frames4{rect(3, 122), rect(20, 122), rect(37, 122), rect(20, 122)},
                Frames4{rect(3, 72), rect(20, 72), rect(37, 72), rect(20, 72)}
            });

        case CharacterSpriteVariant::Bot2:
            return makeSet(
            Frames4{
                rect(20,97),
                rect(20, 47),
                rect(20, 122),
                rect(20, 72)
            },
            WalkSet{
                Frames4{rect(3, 97), rect(20, 97), rect(37, 97), rect(20, 97)},
                Frames4{rect(3, 47), rect(20, 47), rect(37, 47), rect(20, 47)},
                Frames4{rect(3, 122), rect(20, 122), rect(37, 122), rect(20, 122)},
                Frames4{rect(3, 72), rect(20, 72), rect(37, 72), rect(20, 72)}
            });

        case CharacterSpriteVariant::Bot3:
            return makeSet(
            Frames4{
                rect(20,97),
                rect(20, 47),
                rect(20, 122),
                rect(20, 72)
            },
            WalkSet{
                Frames4{rect(3, 97), rect(20, 97), rect(37, 97), rect(20, 97)},
                Frames4{rect(3, 47), rect(20, 47), rect(37, 47), rect(20, 47)},
                Frames4{rect(3, 122), rect(20, 122), rect(37, 122), rect(20, 122)},
                Frames4{rect(3, 72), rect(20, 72), rect(37, 72), rect(20, 72)}
            });
    ;}

    return makeSet(Frames4{rect(0, 0), rect(0, 0), rect(0, 0), rect(0, 0)},
                   WalkSet{});
}

void CharacterView::onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) {
    // TODO: switch (event) to pick the right animation - advance the
    // walk-cycle frame for model_->getFacing() on Moved, switch to the
    // death animation on Died (section 2.2, "Visuals and Aesthetics").
    (void)source;
    if (event == bomberman::logic::EventType::Died) {
        markedForRemoval = true;
    }
}

CharacterView::CharacterView(std::shared_ptr<const bomberman::logic::Character> model,
                             std::shared_ptr<sf::Texture> texture,
                             CharacterAnimationSet animationSet)
    : model(std::move(model)), texture(std::move(texture)), animationSet(std::move(animationSet)) {
    sprite.setTexture(*this->texture);
}

void CharacterView::update(float deltaTime) {
    if (!model) return;

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
    // TODO: sprite_.setPosition(...) via camera.worldToScreen(model_->getPosition()); window.draw(sprite_);
    if (!model || markedForRemoval) return;

    const auto screenPos = camera.worldToScreen(model->getPosition());
    const auto screenSize = camera.worldSizeToScreen(model->getSize());
    const auto facing = model->getFacing();
    const auto facingIdx = dirIndex(facing);

    const sf::IntRect frame = model->isMoving()
        ? animationSet.walk[facingIdx][frameIndex]
        : animationSet.idle[facingIdx];

    sprite.setTextureRect(frame);
    sprite.setOrigin(frame.width * 0.5f, frame.height * 0.5f);
    sprite.setPosition(screenPos.x, screenPos.y);
    sprite.setScale(screenSize.x / static_cast<float>(frame.width),
                    screenSize.y / static_cast<float>(frame.height));

    window.draw(sprite);
}

} // namespace bomberman::representation
