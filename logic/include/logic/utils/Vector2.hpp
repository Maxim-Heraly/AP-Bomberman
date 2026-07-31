#pragma once

namespace bomberman::logic {

/// Simple 2D float vector, fully implemented since it's a generic math
/// utility. Used both for World-space positions (normalized to [-1, 1], see
/// World/EntityModel and Camera) and, in the representation code, for
/// pixel-space positions.
struct Vector2 {
    float x{0.f};
    float y{0.f};

    Vector2() = default;
    Vector2(float x_, float y_) : x(x_), y(y_) {}

    Vector2 operator+(const Vector2& other) const { return {x + other.x, y + other.y}; }
    Vector2 operator-(const Vector2& other) const { return {x - other.x, y - other.y}; }
    Vector2 operator*(float scalar) const { return {x * scalar, y * scalar}; }

    Vector2& operator+=(const Vector2& other) { x += other.x; y += other.y; return *this; }
    Vector2& operator-=(const Vector2& other) { x -= other.x; y -= other.y; return *this; }
};

} // namespace bomberman::logic
