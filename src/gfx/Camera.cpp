#include "gfx/Camera.h"

#include "math/Transform.h"

Camera::Camera()
    : _position(0.0f, 2.0f, 5.0f)
    , _target(0.0f)
    , _up(0.0f, 1.0f, 0.0f)
    , _fovRadians(Transform::radians(60.0f))
    , _nearPlane(0.1f)
    , _farPlane(100.0f)
{}

Vec3 Camera::position() const           { return _position; }
void Camera::setPosition(const Vec3& p) { _position = p; }
Vec3 Camera::target() const             { return _target; }
void Camera::setTarget(const Vec3& t)   { _target = t; }

void Camera::setFov(float radians)           { _fovRadians = radians; }
void Camera::setClipPlanes(float n, float f) { _nearPlane = n; _farPlane = f; }

Mat4 Camera::view() const {
    return Transform::lookAt(_position, _target, _up);
}

Mat4 Camera::projection(float aspect) const {
    return Transform::perspective(_fovRadians, aspect, _nearPlane, _farPlane);
}

Mat4 Camera::viewProjection(float aspect) const {
    return projection(aspect) * view();
}