#pragma once

#include <cmath>
#include <cstddef>
#include <type_traits>

#include "GE/Math/Common.hpp"

namespace GE::Math {

/// 3-component vector. Layout-compatible with glm::vec3 (x, y, z).
template <Component T>
struct TVec3 {
    T x{};
    T y{};
    T z{};

    constexpr TVec3() = default;
    constexpr TVec3(T x_, T y_, T z_) : x(x_), y(y_), z(z_) {}
    explicit constexpr TVec3(T scalar) : x(scalar), y(scalar), z(scalar) {}
    template <Numeric U>
    explicit constexpr TVec3(const TVec3<U>& o)
        : x(static_cast<T>(o.x)),
          y(static_cast<T>(o.y)),
          z(static_cast<T>(o.z)) {}

    [[nodiscard]] constexpr T& operator[](std::size_t i) {
        return i == 0 ? x : i == 1 ? y : z;
    }
    [[nodiscard]] constexpr const T& operator[](std::size_t i) const {
        return i == 0 ? x : i == 1 ? y : z;
    }

    [[nodiscard]] constexpr TVec3 operator+() const { return *this; }
    [[nodiscard]] constexpr TVec3 operator-() const { return {-x, -y, -z}; }

    [[nodiscard]] constexpr TVec3 operator+(TVec3 r) const {
        return {x + r.x, y + r.y, z + r.z};
    }
    [[nodiscard]] constexpr TVec3 operator-(TVec3 r) const {
        return {x - r.x, y - r.y, z - r.z};
    }
    [[nodiscard]] constexpr TVec3 operator*(TVec3 r) const {
        return {x * r.x, y * r.y, z * r.z};
    }
    [[nodiscard]] constexpr TVec3 operator/(TVec3 r) const {
        return {x / r.x, y / r.y, z / r.z};
    }
    [[nodiscard]] constexpr TVec3 operator*(T s) const {
        return {x * s, y * s, z * s};
    }
    [[nodiscard]] constexpr TVec3 operator/(T s) const {
        return {x / s, y / s, z / s};
    }

    constexpr TVec3& operator+=(TVec3 r) {
        x += r.x;
        y += r.y;
        z += r.z;
        return *this;
    }
    constexpr TVec3& operator-=(TVec3 r) {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        return *this;
    }
    constexpr TVec3& operator*=(TVec3 r) {
        x *= r.x;
        y *= r.y;
        z *= r.z;
        return *this;
    }
    constexpr TVec3& operator/=(TVec3 r) {
        x /= r.x;
        y /= r.y;
        z /= r.z;
        return *this;
    }
    constexpr TVec3& operator*=(T s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    constexpr TVec3& operator/=(T s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    [[nodiscard]] friend constexpr bool operator==(TVec3 a, TVec3 b) {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }

    [[nodiscard]] constexpr T Dot(TVec3 r) const {
        return x * r.x + y * r.y + z * r.z;
    }
    [[nodiscard]] constexpr TVec3 Cross(TVec3 r) const {
        return {y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x};
    }
    [[nodiscard]] constexpr T LengthSquared() const { return Dot(*this); }

    [[nodiscard]] T Length() const { return std::sqrt(LengthSquared()); }
    [[nodiscard]] TVec3 Normalized() const {
        T len = Length();
        return len == T(0) ? *this : *this / len;
    }
    [[nodiscard]] constexpr TVec3 Reflect(TVec3 n) const {
        return *this - n * (T(2) * Dot(n));
    }
    /// Refract using Snell's law. `eta` is the ratio of indexes of refraction.
    [[nodiscard]] TVec3 Refract(TVec3 n, T eta) const {
        T k = T(1) - eta * eta * (T(1) - Dot(n) * Dot(n));
        if (k < T(0)) return TVec3(0);
        return *this * eta - n * (eta * Dot(n) + std::sqrt(k));
    }
};

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> operator*(T s, TVec3<T> v) {
    return v * s;
}

template <Numeric T>
[[nodiscard]] constexpr T Dot(TVec3<T> a, TVec3<T> b) {
    return a.Dot(b);
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Cross(TVec3<T> a, TVec3<T> b) {
    return a.Cross(b);
}

template <Numeric T>
[[nodiscard]] constexpr T LengthSquared(TVec3<T> v) {
    return v.LengthSquared();
}

template <Numeric T>
[[nodiscard]] T Length(TVec3<T> v) {
    return v.Length();
}

template <Numeric T>
[[nodiscard]] T Distance(TVec3<T> a, TVec3<T> b) {
    return (a - b).Length();
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Lerp(TVec3<T> a, TVec3<T> b, T t) {
    return a + (b - a) * t;
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Min(TVec3<T> a, TVec3<T> b) {
    return {Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Max(TVec3<T> a, TVec3<T> b) {
    return {Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Clamp(TVec3<T> v, TVec3<T> lo, TVec3<T> hi) {
    return {Clamp(v.x, lo.x, hi.x), Clamp(v.y, lo.y, hi.y),
            Clamp(v.z, lo.z, hi.z)};
}

using Vec3 = TVec3<float>;
using DVec3 = TVec3<double>;
using IVec3 = TVec3<int>;
using UVec3 = TVec3<unsigned>;
using BVec3 = TVec3<bool>;

}  // namespace GE::Math