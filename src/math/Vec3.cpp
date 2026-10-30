#include "math/Vec3.h"

#include <cmath>

Vec3::Vec3(float scalar) : _x(scalar), _y(scalar), _z(scalar) {}
Vec3::Vec3(float x, float y, float z) : _x(x), _y(y), _z(z) {}

float Vec3::x() const { return _x; }
void  Vec3::x(float value) { _x = value; }
float Vec3::y() const { return _y; }
void  Vec3::y(float value) { _y = value; }
float Vec3::z() const { return _z; }
void  Vec3::z(float value) { _z = value; }

Vec3 Vec3::operator+(const Vec3& other) const { return Vec3(_x + other._x, _y + other._y, _z + other._z); }
Vec3 Vec3::operator-(const Vec3& other) const { return Vec3(_x - other._x, _y - other._y, _z - other._z); }
Vec3 Vec3::operator*(float scalar) const      { return Vec3(_x * scalar, _y * scalar, _z * scalar); }
Vec3 Vec3::operator-() const                  { return Vec3(-_x, -_y, -_z); }

Vec3& Vec3::operator+=(const Vec3& other) { _x += other._x; _y += other._y; _z += other._z; return *this; }
Vec3& Vec3::operator-=(const Vec3& other) { _x -= other._x; _y -= other._y; _z -= other._z; return *this; }
Vec3& Vec3::operator*=(float scalar)      { _x *= scalar;   _y *= scalar;   _z *= scalar;   return *this; }

bool Vec3::operator==(const Vec3& other) const {
    return _x == other._x && _y == other._y && _z == other._z;
}
bool Vec3::operator!=(const Vec3& other) const { return !(*this == other); }

float Vec3::dot(const Vec3& other) const {
    return _x * other._x + _y * other._y + _z * other._z;
}

Vec3 Vec3::cross(const Vec3& other) const {
    return Vec3(
        _y * other._z - _z * other._y,
        _z * other._x - _x * other._z,
        _x * other._y - _y * other._x
    );
}

float Vec3::length() const {
    return std::sqrt(_x * _x + _y * _y + _z * _z);
}

Vec3 Vec3::normalized() const {
    float len = length();
    if (len < 1e-8f)
        return Vec3(0.0f, 0.0f, 0.0f);
    return Vec3(_x / len, _y / len, _z / len);
}