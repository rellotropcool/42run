#include "math/Mat4.h"

Mat4::Mat4() {
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            _m[c][r] = 0.0f;
}

Mat4 Mat4::identity() {
    Mat4 result;
    for (int i = 0; i < 4; ++i)
        result._m[i][i] = 1.0f;
    return result;
}

float  Mat4::at(int column, int row) const { return _m[column][row]; }
float& Mat4::at(int column, int row)       { return _m[column][row]; }

const float* Mat4::data() const { return &_m[0][0]; }

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;
    for (int c = 0; c < 4; ++c) {
        for (int r = 0; r < 4; ++r) {
            float sum = 0.0f;
            for (int k = 0; k < 4; ++k)
                sum += _m[k][r] * other._m[c][k];
            result._m[c][r] = sum;
        }
    }
    return result;
}