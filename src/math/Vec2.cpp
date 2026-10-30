#include "math/Vec2.h"

#include <cmath>

Vec2::Vec2(float x, float y) : _x(x), _y(y) {}

float Vec2::x() const { return _x; }
void  Vec2::x(float value) { _x = value; }
float Vec2::y() const { return _y; }
void  Vec2::y(float value) { _y = value; }

Vec2 Vec2::operator+(const Vec2& other) const { return Vec2(_x + other._x, _y + other._y); }
Vec2 Vec2::operator-(const Vec2& other) const { return Vec2(_x - other._x, _y - other._y); }
Vec2 Vec2::operator*(float scalar) const      { return Vec2(_x * scalar, _y * scalar); }

Vec2& Vec2::operator+=(const Vec2& other) { _x += other._x; _y += other._y; return *this; }
Vec2& Vec2::operator-=(const Vec2& other) { _x -= other._x; _y -= other._y; return *this; }
Vec2& Vec2::operator*=(float scalar)      { _x *= scalar;   _y *= scalar;   return *this; }

bool Vec2::operator==(const Vec2& other) const { return _x == other._x && _y == other._y; }
bool Vec2::operator!=(const Vec2& other) const { return !(*this == other); }

float Vec2::dot(const Vec2& other) const { return _x * other._x + _y * other._y; }
float Vec2::length() const { return std::sqrt(_x * _x + _y * _y); }

Vec2 Vec2::normalized() const {
    float len = length();
    if (len < 1e-8f)
        return Vec2(0.0f, 0.0f);
    return Vec2(_x / len, _y / len);
}