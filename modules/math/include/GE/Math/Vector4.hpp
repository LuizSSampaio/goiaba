#pragma once

#include <cmath>
#include <cstddef>
#include <type_traits>

#include "GE/Math/Common.hpp"
#include "GE/Math/Vector3.hpp"

namespace GE::Math {

/// 4-component vector. Layout-compatible with glm::vec4 (x, y, z, w).
template <Component T>
struct TVec4 {
    T x{};
    T y{};
    T z{};
    T w{};

    constexpr TVec4() = default;
    constexpr TVec4(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
    explicit constexpr TVec4(T scalar)
        : x(scalar), y(scalar), z(scalar), w(scalar) {}
    template <Numeric U>
    explicit constexpr TVec4(const TVec4<U>& o)
        : x(static_cast<T>(o.x)),
          y(static_cast<T>(o.y)),
          z(static_cast<T>(o.z)),
          w(static_cast<T>(o.w)) {}
    constexpr TVec4(const TVec3<T>& v, T w_)  // NOLINT implicit vec3->vec4
        : x(v.x), y(v.y), z(v.z), w(w_) {}

    [[nodiscard]] constexpr T& operator[](std::size_t i) {
        return i == 0 ? x : i == 1 ? y : i == 2 ? z : w;
    }
    [[nodiscard]] constexpr const T& operator[](std::size_t i) const {
        return i == 0 ? x : i == 1 ? y : i == 2 ? z : w;
    }

    [[nodiscard]] constexpr TVec3<T> Xyz() const { return {x, y, z}; }

    [[nodiscard]] constexpr TVec4 operator+() const { return *this; }
    [[nodiscard]] constexpr TVec4 operator-() const { return {-x, -y, -z, -w}; }

    [[nodiscard]] constexpr TVec4 operator+(TVec4 r) const {
        return {x + r.x, y + r.y, z + r.z, w + r.w};
    }
    [[nodiscard]] constexpr TVec4 operator-(TVec4 r) const {
        return {x - r.x, y - r.y, z - r.z, w - r.w};
    }
    [[nodiscard]] constexpr TVec4 operator*(TVec4 r) const {
        return {x * r.x, y * r.y, z * r.z, w * r.w};
    }
    [[nodiscard]] constexpr TVec4 operator/(TVec4 r) const {
        return {x / r.x, y / r.y, z / r.z, w / r.w};
    }
    [[nodiscard]] constexpr TVec4 operator*(T s) const {
        return {x * s, y * s, z * s, w * s};
    }
    [[nodiscard]] constexpr TVec4 operator/(T s) const {
        return {x / s, y / s, z / s, w / s};
    }

    constexpr TVec4& operator+=(TVec4 r) {
        x += r.x;
        y += r.y;
        z += r.z;
        w += r.w;
        return *this;
    }
    constexpr TVec4& operator-=(TVec4 r) {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        w -= r.w;
        return *this;
    }
    constexpr TVec4& operator*=(TVec4 r) {
        x *= r.x;
        y *= r.y;
        z *= r.z;
        w *= r.w;
        return *this;
    }
    constexpr TVec4& operator/=(TVec4 r) {
        x /= r.x;
        y /= r.y;
        z /= r.z;
        w /= r.w;
        return *this;
    }
    constexpr TVec4& operator*=(T s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }
    constexpr TVec4& operator/=(T s) {
        x /= s;
        y /= s;
        z /= s;
        w /= s;
        return *this;
    }

    [[nodiscard]] friend constexpr bool operator==(TVec4 a, TVec4 b) {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    [[nodiscard]] constexpr T Dot(TVec4 r) const {
        return x * r.x + y * r.y + z * r.z + w * r.w;
    }
    [[nodiscard]] constexpr T LengthSquared() const { return Dot(*this); }

    [[nodiscard]] T Length() const { return std::sqrt(LengthSquared()); }
    [[nodiscard]] TVec4 Normalized() const {
        T len = Length();
        return len == T(0) ? *this : *this / len;
    }
};

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> operator*(T s, TVec4<T> v) {
    return v * s;
}

template <Numeric T>
[[nodiscard]] constexpr T Dot(TVec4<T> a, TVec4<T> b) {
    return a.Dot(b);
}

template <Numeric T>
[[nodiscard]] constexpr T LengthSquared(TVec4<T> v) {
    return v.LengthSquared();
}

template <Numeric T>
[[nodiscard]] T Length(TVec4<T> v) {
    return v.Length();
}

template <Numeric T>
[[nodiscard]] T Distance(TVec4<T> a, TVec4<T> b) {
    return (a - b).Length();
}

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> Lerp(TVec4<T> a, TVec4<T> b, T t) {
    return a + (b - a) * t;
}

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> Min(TVec4<T> a, TVec4<T> b) {
    return {Min(a.x, b.x), Min(a.y, b.y), Min(a.z, b.z), Min(a.w, b.w)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> Max(TVec4<T> a, TVec4<T> b) {
    return {Max(a.x, b.x), Max(a.y, b.y), Max(a.z, b.z), Max(a.w, b.w)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> Clamp(TVec4<T> v, TVec4<T> lo, TVec4<T> hi) {
    return {Clamp(v.x, lo.x, hi.x), Clamp(v.y, lo.y, hi.y),
            Clamp(v.z, lo.z, hi.z), Clamp(v.w, lo.w, hi.w)};
}

using Vec4 = TVec4<float>;
using DVec4 = TVec4<double>;
using IVec4 = TVec4<int>;
using UVec4 = TVec4<unsigned>;
using BVec4 = TVec4<bool>;

}  // namespace GE::Math