#ifndef CAMERA_H
#define CAMERA_H

#include "math/Mat4.h"
#include "math/Vec3.h"

class Camera {
    private:
        Vec3  _position;
        Vec3  _target;
        Vec3  _up;
        float _fovRadians;
        float _nearPlane;
        float _farPlane;
    public:
        Camera();

        Vec3 position() const;
        void setPosition(const Vec3& position);

        Vec3 target() const;
        void setTarget(const Vec3& target);

        void setFov(float radians);
        void setClipPlanes(float nearPlane, float farPlane);

        Mat4 view()                       const;
        Mat4 projection(float aspect)     const;
        Mat4 viewProjection(float aspect) const;
};

#endif