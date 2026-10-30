#ifndef VEC2_H
#define VEC2_H

class Vec2 {
    private:
        float _x = 0.0f;
        float _y = 0.0f;
    public:
        Vec2() = default;
        Vec2(float x, float y);

        float x() const;
        void  x(float value);
        float y() const;
        void  y(float value);

        Vec2  operator+(const Vec2& other) const;
        Vec2  operator-(const Vec2& other) const;
        Vec2  operator*(float scalar)      const;
        Vec2& operator+=(const Vec2& other);
        Vec2& operator-=(const Vec2& other);
        Vec2& operator*=(float scalar);
        bool  operator==(const Vec2& other) const;
        bool  operator!=(const Vec2& other) const;

        float dot(const Vec2& other) const;
        float length()               const;
        Vec2  normalized()           const;
};

#endif