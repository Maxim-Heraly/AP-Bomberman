#include "representation/views/BombView.hpp"
#include <algorithm>

namespace bomberman::representation {

    namespace {
        constexpr float kFuseFrameSeconds = 0.12f;
        constexpr float kExplosionFrameSeconds = 0.06f;

        constexpr int kBombFrameWidth = 16;
        constexpr int kBombFrameHeight = 16;
        constexpr int kExplosionFrameWidth = 16;
        constexpr int kExplosionFrameHeight = 16;

        sf::IntRect bombRect(int left, int top) {
            return {left, top, kBombFrameWidth, kBombFrameHeight};
        }

        sf::IntRect explosionRect(int left, int top) {
            return {left, top, kExplosionFrameWidth, kExplosionFrameHeight};
        }
    } // namespace

    BombAnimationSet makeBombAnimationSet() {
        const std::array<sf::IntRect, 4> bombFrames{
            bombRect(103, 117),
            bombRect(120, 117),
            bombRect(137, 117),
            bombRect(120, 117)
        };

        const std::array<sf::IntRect, 9> center {
            explosionRect(69, 83), explosionRect(120, 66), explosionRect(103, 66),
            explosionRect(86, 66), explosionRect(69, 66), explosionRect(86, 66),
            explosionRect(103, 66), explosionRect(120, 66), explosionRect(69, 83)};
        const std::array<sf::IntRect, 9> endUp {
            explosionRect(69, 49), explosionRect(86, 49), explosionRect(103, 49),
            explosionRect(120, 49), explosionRect(120, 32), explosionRect(120, 49),
            explosionRect(103, 49), explosionRect(86, 49), explosionRect(69, 49)};
        const std::array<sf::IntRect, 9> endDown {
            explosionRect(52, 49), explosionRect(69, 100), explosionRect(120, 100),
            explosionRect(103, 100), explosionRect(86, 100), explosionRect(103, 100),
            explosionRect(120, 100), explosionRect(69, 100), explosionRect(52, 49)};
        const std::array<sf::IntRect, 9> endLeft {
            explosionRect(1, 32), explosionRect(18, 32), explosionRect(35, 32),
            explosionRect(52, 32), explosionRect(69, 32), explosionRect(52, 32),
            explosionRect(35, 32), explosionRect(18, 32), explosionRect(1, 32)};
        const std::array<sf::IntRect, 9> endRight {
            explosionRect(52, 83), explosionRect(52, 66), explosionRect(18, 100),
            explosionRect(18, 83), explosionRect(18, 66), explosionRect(18, 83),
            explosionRect(18, 100), explosionRect(52, 66), explosionRect(52, 83)};
        const std::array<sf::IntRect, 9> bodyVertical {
            explosionRect(52, 100), explosionRect(35, 100), explosionRect(120, 83),
            explosionRect(103, 83), explosionRect(86, 83), explosionRect(103, 83),
            explosionRect(120, 83), explosionRect(35, 100), explosionRect(52, 100)};
        const std::array<sf::IntRect, 9> bodyHorizontal {
            explosionRect(35, 83), explosionRect(35, 66), explosionRect(1, 100),
            explosionRect(1, 83), explosionRect(1, 66), explosionRect(1, 83),
            explosionRect(1, 100), explosionRect(35, 66), explosionRect(35, 83)};


        return {bombFrames, center, endUp, endDown, endLeft, endRight, bodyVertical, bodyHorizontal};
    }

    BombView::BombView(std::shared_ptr<const bomberman::logic::Bomb> model,
                       std::shared_ptr<sf::Texture> texture,
                       BombAnimationSet animations)
        : model(std::move(model)), texture(std::move(texture)), animations(std::move(animations)) {
        sprite.setTexture(*this->texture);
    }

    void BombView::update(float deltaTime) {
        if (!model || phase == Phase::Done) return;

        if (phase == Phase::Fuse) {
            if (model->hasExploded()) {
                phase = Phase::Explosion;
                explosionFrameTimer = 0.f;
                explosionFrameIndex = 0;
                return;
            }

            fuseFrameTimer += deltaTime;
            while (fuseFrameTimer >= kFuseFrameSeconds) {
                fuseFrameTimer -= kFuseFrameSeconds;
                fuseFrameIndex = (fuseFrameIndex + 1) % animations.bomb.size();
            }
            return;
        }

        if (phase == Phase::Explosion) {
            explosionFrameTimer += deltaTime;
            while (explosionFrameTimer >= kExplosionFrameSeconds) {
                explosionFrameTimer -= kExplosionFrameSeconds;
                if (explosionFrameIndex + 1 < animations.center.size()) {
                    ++explosionFrameIndex;
                } else {
                    phase = Phase::Done;
                    markedForRemoval = true;
                    break;
                }
            }
        }
    }

    void BombView::onNotify(const bomberman::logic::Subject& source, bomberman::logic::EventType event) {
        (void)source;
        if (event == bomberman::logic::EventType::BombExploded && phase != Phase::Done) {
            phase = Phase::Explosion;
            explosionFrameTimer = 0.f;
            explosionFrameIndex = 0;
        }
    }

    void BombView::drawFrame(sf::RenderWindow& window, const Camera& camera,
                             const sf::IntRect& frame, const bomberman::logic::Vector2& worldPos) const {
        const auto screenPos = camera.worldToScreen(worldPos);
        const auto screenSize = camera.worldSizeToScreen(model->getSize());

        sprite.setTextureRect(frame);
        sprite.setOrigin(frame.width * 0.5f, frame.height * 0.5f);
        sprite.setPosition(screenPos.x, screenPos.y);
        sprite.setScale(screenSize.x / static_cast<float>(frame.width),
                        screenSize.y / static_cast<float>(frame.height));
        window.draw(sprite);
    }

    void BombView::draw(sf::RenderWindow& window, const Camera& camera) {
        if (!model || markedForRemoval || phase == Phase::Done) return;

        const auto center = model->getPosition();

        if (phase == Phase::Fuse) {
            drawFrame(window, camera, animations.bomb[fuseFrameIndex], center);
            return;
        }

        // Explosion phase
        const auto frameIdx = std::min(explosionFrameIndex, animations.center.size() - 1);
        drawFrame(window, camera, animations.center[frameIdx], center);

        const int radius = std::max(1, model->getRadius());
        const auto tile = model->getSize();

        for (int step = 1; step <= radius; ++step) {
            const float dx = tile.x * static_cast<float>(step);
            const float dy = tile.y * static_cast<float>(step);
            const bool isEnd = (step == radius);

            const sf::IntRect& upFrame = isEnd ? animations.endUp[frameIdx] : animations.bodyVertical[frameIdx];
            const sf::IntRect& downFrame = isEnd ? animations.endDown[frameIdx] : animations.bodyVertical[frameIdx];
            const sf::IntRect& leftFrame = isEnd ? animations.endLeft[frameIdx] : animations.bodyHorizontal[frameIdx];
            const sf::IntRect& rightFrame = isEnd ? animations.endRight[frameIdx] : animations.bodyHorizontal[frameIdx];

            drawFrame(window, camera, upFrame, {center.x, center.y - dy});
            drawFrame(window, camera, downFrame, {center.x, center.y + dy});
            drawFrame(window, camera, leftFrame, {center.x - dx, center.y});
            drawFrame(window, camera, rightFrame, {center.x + dx, center.y});
        }
    }

} // namespace bomberman::representation