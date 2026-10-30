#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "math/Mat4.h"
#include "math/Vec3.h"

class Transform {
    public:
        static float radians(float degrees);

        static Mat4 translate(const Mat4& m, const Vec3& t);
        static Mat4 rotate(const Mat4& m, float angleRad, const Vec3& axis);
        static Mat4 scale(const Mat4& m, const Vec3& s);

        static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
        static Mat4 perspective(float fovRad, float aspect,
                                float nearPlane, float farPlane);
};

#endif