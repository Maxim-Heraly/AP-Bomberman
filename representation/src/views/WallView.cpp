#include "representation/views/WallView.hpp"
#include "logic/utils/Stopwatch.hpp"

namespace bomberman::representation {

    using logic::EventType;
    using logic::Stopwatch;
    using logic::Subject;
    using logic::Vector2;
    using logic::Wall;

    namespace {
        constexpr int kWallFrameWidth = 16;
        constexpr int kWallFrameHeight = 16;

        sf::IntRect rect(int left, int top) {
            return {left, top, kWallFrameWidth, kWallFrameHeight};
        }

        WallAnimationSet makeSet(const std::array<sf::IntRect, 1>& idle, const std::array<sf::IntRect, 6>& breaking) {
            return {idle, breaking};
        }
    }

    WallAnimationSet makeWallAnimationSet(WallSpriteVariant variant) {
        switch (variant) {
            case WallSpriteVariant::Indestructible:
                return makeSet({rect(18, 15)}, {});

            case WallSpriteVariant::Destructible:
                return makeSet(
                    {rect(35, 15)},
                    {rect(1, 117), rect(18, 117), rect(35, 117), rect(52, 117), rect(69, 117), rect(86, 117)}
                );
        }
        return makeSet({rect(18, 15)}, {});
    }

    WallView::WallView(std::shared_ptr<const Wall> model,
                       std::shared_ptr<sf::Texture> texture,
                       const WallAnimationSet &animationSet)
        : model(std::move(model)), texture(std::move(texture)), animationSet(animationSet) {
        sprite.setTexture(*this->texture);
    }

    void WallView::onNotify(const Subject& /*source*/, const EventType event) {
        if (event == EventType::BlockDestroyed) {
            breaking = true;
        }
    }

    void WallView::draw(sf::RenderWindow& window, const Camera& camera) {
        if (breaking) {
            breakTimer += Stopwatch::getInstance().getDeltaTime();
            if (breakTimer >= kBreakDuration) {
                markedForRemoval = true;
                return;
            }
        }

        const Vector2 screenPos = camera.worldToScreen(model->getPosition());
        const Vector2 screenSize = camera.worldSizeToScreen(model->getSize());

        sprite.setTexture(*this->texture);
        sprite.setTextureRect(breaking ? animationSet.breaking[std::min(static_cast<std::size_t>(breakTimer / (kBreakDuration / animationSet.breaking.size())), animationSet.breaking.size() - 1)] : animationSet.idle[0]);
        sprite.setScale(screenSize.x / kWallFrameWidth, screenSize.y / kWallFrameHeight);
        sprite.setOrigin(kWallFrameWidth * 0.5f, kWallFrameHeight * 0.5f);
        sprite.setPosition(screenPos.x, screenPos.y);
        window.draw(sprite);
    }

} // namespace bomberman::representation