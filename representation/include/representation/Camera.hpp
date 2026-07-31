#pragma once

#include "logic/utils/Vector2.hpp"

namespace bomberman::representation {

/**
 * @brief Projects normalized World coordinates ([-1, 1] on both axes, see
 * World/EntityModel, section 3.1 "Camera") to pixel coordinates on the
 * current window. Implemented manually (no SFML transforms) so the logic
 * library never has to know about pixels, and the game could theoretically
 * be driven by a completely different renderer.
 *
 * A minimal, centered/uniform-scale implementation is provided below - feel
 * free to replace it (e.g. to support a non-square arena, or letterboxing
 * when the window is resized to a different aspect ratio).
 */
class Camera {
public:
    Camera(unsigned int windowWidth, unsigned int windowHeight)
        : windowWidth(windowWidth), windowHeight(windowHeight) {}

    void setWindowSize(unsigned int width, unsigned int height) {
        windowWidth = width;
        windowHeight = height;
    }

    /// Maps world [-1, 1] -> pixels [0, windowWidth_] / [0, windowHeight_].
    bomberman::logic::Vector2 worldToScreen(const bomberman::logic::Vector2& world) const {
        const float px = (world.x + 1.f) * 0.5f * static_cast<float>(windowWidth);
        const float py = (world.y + 1.f) * 0.5f * static_cast<float>(windowHeight);
        return {px, py};
    }

    /// TODO: use this (e.g. in each EntityView::draw()) to compute the
    /// on-screen width/height of a sprite, so it scales correctly if the
    /// window is resized.
    bomberman::logic::Vector2 worldSizeToScreen(const bomberman::logic::Vector2& size) const {
        return {size.x * 0.5f * static_cast<float>(windowWidth),
                size.y * 0.5f * static_cast<float>(windowHeight)};
    }

private:
    unsigned int windowWidth;
    unsigned int windowHeight;
};

} // namespace bomberman::representation
