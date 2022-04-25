#ifndef FLINT_VEC3_H
#define FLINT_VEC3_H

#include "vec2.h"

namespace Flint {
    template<typename T>
    struct Vec3 {
        T x = 0;
        T y = 0;
        T z = 0;

        Vec3() = default;

        explicit Vec3(T p_s) : x(p_s), y(p_s), z(p_s) {};

        Vec3(T p_x, T p_y, T p_z) : x(p_x), y(p_y), z(p_z) {};

        static inline Vec3 zero() {
            return {0};
        }

        Vec2<T> xy() const {
            return {x, y};
        }

        inline Vec3 operator*(float s) const {
            return Vec3{x * s, y * s, z * s};
        }

        inline void operator+=(const Vec3 &b) {
            x += b.x;
            y += b.y;
            z += b.z;
        }
    };
}

#endif //FLINT_VEC3_H