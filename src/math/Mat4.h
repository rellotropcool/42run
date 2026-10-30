#ifndef MAT4_H
#define MAT4_H

class Mat4 {
    private:
        float _m[4][4] = {};
    public:
        Mat4();

        static Mat4 identity();

        float        at(int column, int row) const;
        float&       at(int column, int row);
        const float* data() const;

        Mat4 operator*(const Mat4& other) const;
};

#endif