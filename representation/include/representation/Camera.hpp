#pragma once

#include "logic/utils/Vector2.hpp"

namespace bomberman::representation {

    /**
     * @brief Projects normalized World coordinates ([-1, 1] on both axes, see
     * World/EntityModel, section 3.1 "Camera") to pixel coordinates on the
     * current window.
     *
     * The world is always a rectangle (World::generateArena() lays out a 15x13
     * grid of tiles centered on the origin, exactly filling
     * [-1, 1] on both axes).
     *
     * Unlike a letterboxed/pillarboxed camera, this maps the full [-1, 1] range
     * independently on X and Y to the full window width/height, so the arena's
     * outer walls always sit flush against the window's edges, and the whole
     * scene stretches/scales to fill the window exactly - no bars, no gaps.
     */
    class Camera {
    public:
        Camera(unsigned int windowWidth, unsigned int windowHeight)
            : windowWidth(windowWidth), windowHeight(windowHeight) {}

        void setWindowSize(unsigned int width, unsigned int height) {
            windowWidth = width;
            windowHeight = height;
        }

        /// Maps world [-1, 1] -> pixels, filling the entire window on both axes.
        logic::Vector2 worldToScreen(const logic::Vector2& world) const {
            const float px = (world.x + 1.f) * 0.5f * static_cast<float>(windowWidth);
            const float py = (world.y + 1.f) * 0.5f * static_cast<float>(windowHeight);
            return {px, py};
        }

        /// Use this (e.g. in each EntityView::draw()) to compute the on-screen
        /// width/height of a sprite, so it scales correctly - independently on
        /// each axis - if the window is resized.
        logic::Vector2 worldSizeToScreen(const logic::Vector2& size) const {
            return {size.x * 0.5f * static_cast<float>(windowWidth),
                    size.y * 0.5f * static_cast<float>(windowHeight)};
        }

    private:
        unsigned int windowWidth;
        unsigned int windowHeight;
    };

} // namespace bomberman::representation