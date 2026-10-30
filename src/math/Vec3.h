#ifndef VEC3_H
#define VEC3_H

class Vec3 {
    private:
        float _x = 0.0f;
        float _y = 0.0f;
        float _z = 0.0f;
    public:
        Vec3() = default;
        Vec3(float scalar);
        Vec3(float x, float y, float z);

        float x() const;
        void  x(float value);
        float y() const;
        void  y(float value);
        float z() const;
        void  z(float value);

        Vec3  operator+(const Vec3& other) const;
        Vec3  operator-(const Vec3& other) const;
        Vec3  operator*(float scalar)      const;
        Vec3  operator-()                  const;
        Vec3& operator+=(const Vec3& other);
        Vec3& operator-=(const Vec3& other);
        Vec3& operator*=(float scalar);
        bool  operator==(const Vec3& other) const;
        bool  operator!=(const Vec3& other) const;

        float dot(const Vec3& other)   const;
        Vec3  cross(const Vec3& other) const;
        float length()                 const;
        Vec3  normalized()             const;
};

#endif