#pragma once

#include <cmath>
#include <cstddef>
#include <type_traits>

#include "GE/Math/Common.hpp"

namespace GE::Math {

/// 2-component vector. Layout-compatible with glm::vec2 (x, y) so the private
/// bridge layer can bit_cast between them at zero cost.
template <Component T>
struct TVec2 {
    T x{};
    T y{};

    constexpr TVec2() = default;
    constexpr TVec2(T x_, T y_) : x(x_), y(y_) {}
    explicit constexpr TVec2(T scalar) : x(scalar), y(scalar) {}
    template <Numeric U>
    explicit constexpr TVec2(const TVec2<U>& o)
        : x(static_cast<T>(o.x)), y(static_cast<T>(o.y)) {}

    [[nodiscard]] constexpr T& operator[](std::size_t i) {
        return i == 0 ? x : y;
    }
    [[nodiscard]] constexpr const T& operator[](std::size_t i) const {
        return i == 0 ? x : y;
    }

    [[nodiscard]] constexpr TVec2 operator+() const { return *this; }
    [[nodiscard]] constexpr TVec2 operator-() const { return {-x, -y}; }

    [[nodiscard]] constexpr TVec2 operator+(TVec2 r) const {
        return {x + r.x, y + r.y};
    }
    [[nodiscard]] constexpr TVec2 operator-(TVec2 r) const {
        return {x - r.x, y - r.y};
    }
    [[nodiscard]] constexpr TVec2 operator*(TVec2 r) const {
        return {x * r.x, y * r.y};
    }
    [[nodiscard]] constexpr TVec2 operator/(TVec2 r) const {
        return {x / r.x, y / r.y};
    }
    [[nodiscard]] constexpr TVec2 operator*(T s) const {
        return {x * s, y * s};
    }
    [[nodiscard]] constexpr TVec2 operator/(T s) const {
        return {x / s, y / s};
    }

    constexpr TVec2& operator+=(TVec2 r) {
        x += r.x;
        y += r.y;
        return *this;
    }
    constexpr TVec2& operator-=(TVec2 r) {
        x -= r.x;
        y -= r.y;
        return *this;
    }
    constexpr TVec2& operator*=(TVec2 r) {
        x *= r.x;
        y *= r.y;
        return *this;
    }
    constexpr TVec2& operator/=(TVec2 r) {
        x /= r.x;
        y /= r.y;
        return *this;
    }
    constexpr TVec2& operator*=(T s) {
        x *= s;
        y *= s;
        return *this;
    }
    constexpr TVec2& operator/=(T s) {
        x /= s;
        y /= s;
        return *this;
    }

    [[nodiscard]] friend constexpr bool operator==(TVec2 a, TVec2 b) {
        return a.x == b.x && a.y == b.y;
    }

    [[nodiscard]] constexpr T Dot(TVec2 r) const { return x * r.x + y * r.y; }
    [[nodiscard]] constexpr T LengthSquared() const { return Dot(*this); }

    /// Length is inline (std::sqrt is not constexpr until C++26).
    [[nodiscard]] T Length() const { return std::sqrt(LengthSquared()); }
    [[nodiscard]] TVec2 Normalized() const {
        T len = Length();
        return len == T(0) ? *this : *this / len;
    }
    [[nodiscard]] constexpr TVec2 Reflect(TVec2 n) const {
        return *this - n * (T(2) * Dot(n));
    }
};

template <Numeric T>
[[nodiscard]] constexpr TVec2<T> operator*(T s, TVec2<T> v) {
    return v * s;
}

template <Numeric T>
[[nodiscard]] constexpr T Dot(TVec2<T> a, TVec2<T> b) {
    return a.Dot(b);
}

template <Numeric T>
[[nodiscard]] constexpr T LengthSquared(TVec2<T> v) {
    return v.LengthSquared();
}

template <Numeric T>
[[nodiscard]] T Length(TVec2<T> v) {
    return v.Length();
}

template <Numeric T>
[[nodiscard]] T Distance(TVec2<T> a, TVec2<T> b) {
    return (a - b).Length();
}

template <Numeric T>
[[nodiscard]] constexpr TVec2<T> Lerp(TVec2<T> a, TVec2<T> b, T t) {
    return a + (b - a) * t;
}

template <Numeric T>
[[nodiscard]] constexpr TVec2<T> Min(TVec2<T> a, TVec2<T> b) {
    return {Min(a.x, b.x), Min(a.y, b.y)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec2<T> Max(TVec2<T> a, TVec2<T> b) {
    return {Max(a.x, b.x), Max(a.y, b.y)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec2<T> Clamp(TVec2<T> v, TVec2<T> lo, TVec2<T> hi) {
    return {Clamp(v.x, lo.x, hi.x), Clamp(v.y, lo.y, hi.y)};
}

using Vec2 = TVec2<float>;
using DVec2 = TVec2<double>;
using IVec2 = TVec2<int>;
using UVec2 = TVec2<unsigned>;
using BVec2 = TVec2<bool>;

}  // namespace GE::Math