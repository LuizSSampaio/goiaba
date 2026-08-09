#pragma once

#include <cmath>
#include <cstddef>

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
    explicit constexpr TVec4(const TVec4<U>& o_)
        : x(static_cast<T>(o_.x)),
          y(static_cast<T>(o_.y)),
          z(static_cast<T>(o_.z)),
          w(static_cast<T>(o_.w)) {}
    constexpr TVec4(const TVec3<T>& v, T w_)  // NOLINT implicit vec3->vec4
        : x(v.x), y(v.y), z(v.z), w(w_) {}

    [[nodiscard]] constexpr T& operator[](std::size_t index) {
        constexpr auto noY = index == 2 ? z : w;
        constexpr auto noX = index == 1 ? y : noY;
        return index == 0 ? x : noX;
    }
    [[nodiscard]] constexpr const T& operator[](std::size_t index) const {
        constexpr auto noY = index == 2 ? z : w;
        constexpr auto noX = index == 1 ? y : noY;
        return index == 0 ? x : noX;
    }

    [[nodiscard]] constexpr TVec3<T> Xyz() const { return {x, y, z}; }

    [[nodiscard]] constexpr TVec4 operator+() const { return *this; }
    [[nodiscard]] constexpr TVec4 operator-() const { return {-x, -y, -z, -w}; }

    [[nodiscard]] constexpr TVec4 operator+(TVec4 rhs) const {
        return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
    }
    [[nodiscard]] constexpr TVec4 operator-(TVec4 rhs) const {
        return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
    }
    [[nodiscard]] constexpr TVec4 operator*(TVec4 rhs) const {
        return {x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w};
    }
    [[nodiscard]] constexpr TVec4 operator/(TVec4 rhs) const {
        return {x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w};
    }
    [[nodiscard]] constexpr TVec4 operator*(T val) const {
        return {x * val, y * val, z * val, w * val};
    }
    [[nodiscard]] constexpr TVec4 operator/(T val) const {
        return {x / val, y / val, z / val, w / val};
    }

    constexpr TVec4& operator+=(TVec4 rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }
    constexpr TVec4& operator-=(TVec4 rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }
    constexpr TVec4& operator*=(TVec4 rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        w *= rhs.w;
        return *this;
    }
    constexpr TVec4& operator/=(TVec4 rhs) {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        w /= rhs.w;
        return *this;
    }
    constexpr TVec4& operator*=(T val) {
        x *= val;
        y *= val;
        z *= val;
        w *= val;
        return *this;
    }
    constexpr TVec4& operator/=(T val) {
        x /= val;
        y /= val;
        z /= val;
        w /= val;
        return *this;
    }

    [[nodiscard]] friend constexpr bool operator==(TVec4 vecA, TVec4 vecB) {
        return vecA.x == vecB.x && vecA.y == vecB.y && vecA.z == vecB.z &&
               vecA.w == vecB.w;
    }

    [[nodiscard]] constexpr T Dot(TVec4 rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }
    [[nodiscard]] constexpr T LengthSquared() const { return Dot(*this); }

    [[nodiscard]] T Length() const { return std::sqrt(LengthSquared()); }
    [[nodiscard]] TVec4 Normalized() const {
        T len = Length();
        return len == T(0) ? *this : *this / len;
    }
};

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> operator*(T val, TVec4<T> vec) {
    return vec * val;
}

template <Numeric T>
[[nodiscard]] constexpr T Dot(TVec4<T> vecA, TVec4<T> vecB) {
    return vecA.Dot(vecB);
}

template <Numeric T>
[[nodiscard]] constexpr T LengthSquared(TVec4<T> vec) {
    return vec.LengthSquared();
}

template <Numeric T>
[[nodiscard]] T Length(TVec4<T> vec) {
    return vec.Length();
}

template <Numeric T>
[[nodiscard]] T Distance(TVec4<T> vecA, TVec4<T> vecB) {
    return (vecA - vecB).Length();
}

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> Lerp(TVec4<T> vecA, TVec4<T> vecB, T time) {
    return vecA + (vecB - vecA) * time;
}

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> Min(TVec4<T> vecA, TVec4<T> vecB) {
    return {Min(vecA.x, vecB.x), Min(vecA.y, vecB.y), Min(vecA.z, vecB.z), Min(vecA.w, vecB.w)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> Max(TVec4<T> vecA, TVec4<T> vecB) {
    return {Max(vecA.x, vecB.x), Max(vecA.y, vecB.y), Max(vecA.z, vecB.z), Max(vecA.w, vecB.w)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec4<T> Clamp(TVec4<T> vec, TVec4<T> lo, TVec4<T> hi) {
    return {Clamp(vec.x, lo.x, hi.x), Clamp(vec.y, lo.y, hi.y),
            Clamp(vec.z, lo.z, hi.z), Clamp(vec.w, lo.w, hi.w)};
}

using Vec4 = TVec4<float>;
using DVec4 = TVec4<double>;
using IVec4 = TVec4<int>;
using UVec4 = TVec4<unsigned>;
using BVec4 = TVec4<bool>;

}  // namespace GE::Math
