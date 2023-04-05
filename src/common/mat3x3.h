#ifndef FLINT_MAT3X3_H
#define FLINT_MAT3X3_H

#include "pathfinder.h"

namespace Flint::Math {

/// A 3x3 matrix, in column-major order.
template <typename T>
struct Mat3x3 {
    T v[9] = {0};

    Mat3x3() = default;

    explicit Mat3x3(T s) {
        for (int i = 0; i < 3; i++) {
            v[i * 3 + i] = s;
        }
    }

    static Mat3x3 from_scale(const Pathfinder::Vec3F &scale) {
        auto mat = Mat3x3(1);

        mat.v[0] = scale.x;
        mat.v[4] = scale.y;
        mat.v[8] = scale.z;

        return mat;
    }

    inline Mat3x3 operator*(const Mat3x3 &other) const {
        auto mat = Mat3x3();

        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                auto index = i + j * 3;
                mat.v[index] = 0;
                for (int k = 0; k < 3; k++) {
                    mat.v[index] += v[i + k * 3] * other.v[k + j * 3];
                }
            }
        }

        return mat;
    }

    inline Mat3x3 scale(const Pathfinder::Vec3<T> &scale) const {
        return *this * Mat3x3<T>::from_scale(scale);
    }
};

} // namespace Flint::Math

#endif // FLINT_MAT3X3_H
