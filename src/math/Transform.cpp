#include "math/Transform.h"

#include <cmath>

float Transform::radians(float degrees) {
    return degrees * 3.14159265358979323846f / 180.0f;
}

Mat4 Transform::translate(const Mat4& m, const Vec3& t) {
    Mat4 result = m;
    for (int r = 0; r < 4; ++r) {
        result.at(3, r) = m.at(0, r) * t.x()
                        + m.at(1, r) * t.y()
                        + m.at(2, r) * t.z()
                        + m.at(3, r);
    }
    return result;
}

Mat4 Transform::rotate(const Mat4& m, float angleRad, const Vec3& axis) {
    Vec3 a = axis.normalized();
    float c = std::cos(angleRad);
    float s = std::sin(angleRad);
    float t = 1.0f - c;
    float ux = a.x(), uy = a.y(), uz = a.z();

    Mat4 r = Mat4::identity();
    r.at(0, 0) = c + ux * ux * t;
    r.at(0, 1) = uy * ux * t + uz * s;
    r.at(0, 2) = uz * ux * t - uy * s;
    r.at(1, 0) = ux * uy * t - uz * s;
    r.at(1, 1) = c + uy * uy * t;
    r.at(1, 2) = uz * uy * t + ux * s;
    r.at(2, 0) = ux * uz * t + uy * s;
    r.at(2, 1) = uy * uz * t - ux * s;
    r.at(2, 2) = c + uz * uz * t;
    return m * r;
}

Mat4 Transform::scale(const Mat4& m, const Vec3& s) {
    Mat4 result = m;
    for (int r = 0; r < 4; ++r) {
        result.at(0, r) = m.at(0, r) * s.x();
        result.at(1, r) = m.at(1, r) * s.y();
        result.at(2, r) = m.at(2, r) * s.z();
    }
    return result;
}

Mat4 Transform::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = (center - eye).normalized();
    Vec3 s = f.cross(up).normalized();
    Vec3 u = s.cross(f);

    Mat4 result = Mat4::identity();
    result.at(0, 0) =  s.x();
    result.at(0, 1) =  s.y();
    result.at(0, 2) =  s.z();
    result.at(0, 3) = -s.dot(eye);
    result.at(1, 0) =  u.x();
    result.at(1, 1) =  u.y();
    result.at(1, 2) =  u.z();
    result.at(1, 3) = -u.dot(eye);
    result.at(2, 0) = -f.x();
    result.at(2, 1) = -f.y();
    result.at(2, 2) = -f.z();
    result.at(2, 3) =  f.dot(eye);
    return result;
}

Mat4 Transform::perspective(float fovRad, float aspect,
                            float nearPlane, float farPlane)
{
    float f = 1.0f / std::tan(fovRad * 0.5f);

    Mat4 result;
    result.at(0, 0) = f / aspect;
    result.at(1, 1) = f;
    result.at(2, 2) = (farPlane + nearPlane) / (nearPlane - farPlane);
    result.at(2, 3) = -1.0f;
    result.at(3, 2) = (2.0f * farPlane * nearPlane) / (nearPlane - farPlane);
    return result;
}