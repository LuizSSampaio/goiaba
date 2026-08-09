#pragma once

#include <GE/Math/Common.hpp>
#include <cmath>
#include <cstddef>

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
    explicit constexpr TVec3(const TVec3<U>& o_)
        : x(static_cast<T>(o_.x)),
          y(static_cast<T>(o_.y)),
          z(static_cast<T>(o_.z)) {}

    [[nodiscard]] constexpr T& operator[](std::size_t index) {
        constexpr auto notX = (index == 1 ? y : z);
        return index == 0 ? x : notX;
    }
    [[nodiscard]] constexpr const T& operator[](std::size_t index) const {
        constexpr auto notX = (index == 1 ? y : z);
        return index == 0 ? x : notX;
    }

    [[nodiscard]] constexpr TVec3 operator+() const { return *this; }
    [[nodiscard]] constexpr TVec3 operator-() const { return {-x, -y, -z}; }

    [[nodiscard]] constexpr TVec3 operator+(TVec3 rhs) const {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }
    [[nodiscard]] constexpr TVec3 operator-(TVec3 rhs) const {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }
    [[nodiscard]] constexpr TVec3 operator*(TVec3 rhs) const {
        return {x * rhs.x, y * rhs.y, z * rhs.z};
    }
    [[nodiscard]] constexpr TVec3 operator/(TVec3 rhs) const {
        return {x / rhs.x, y / rhs.y, z / rhs.z};
    }
    [[nodiscard]] constexpr TVec3 operator*(T val) const {
        return {x * val, y * val, z * val};
    }
    [[nodiscard]] constexpr TVec3 operator/(T val) const {
        return {x / val, y / val, z / val};
    }

    constexpr TVec3& operator+=(TVec3 rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }
    constexpr TVec3& operator-=(TVec3 rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }
    constexpr TVec3& operator*=(TVec3 rhs) {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }
    constexpr TVec3& operator/=(TVec3 rhs) {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }
    constexpr TVec3& operator*=(T val) {
        x *= val;
        y *= val;
        z *= val;
        return *this;
    }
    constexpr TVec3& operator/=(T val) {
        x /= val;
        y /= val;
        z /= val;
        return *this;
    }

    [[nodiscard]] friend constexpr bool operator==(TVec3 vecA, TVec3 vecB) {
        return vecA.x == vecB.x && vecA.y == vecB.y && vecA.z == vecB.z;
    }

    [[nodiscard]] constexpr T Dot(TVec3 vec) const {
        return (x * vec.x) + (y * vec.y) + (z * vec.z);
    }
    [[nodiscard]] constexpr TVec3 Cross(TVec3 vec) const {
        return {(y * vec.z) - (z * vec.y), (z * vec.x) - (x * vec.z),
                (x * vec.y) - (y * vec.x)};
    }
    [[nodiscard]] constexpr T LengthSquared() const { return Dot(*this); }

    [[nodiscard]] T Length() const { return std::sqrt(LengthSquared()); }
    [[nodiscard]] TVec3 Normalized() const {
        T len = Length();
        return len == T(0) ? *this : *this / len;
    }
    [[nodiscard]] constexpr TVec3 Reflect(TVec3 n) const {
        return *this - (n * (T(2) * Dot(n)));
    }
    /// Refract using Snell's law. `eta` is the ratio of indexes of refraction.
    [[nodiscard]] TVec3 Refract(TVec3 n, T eta) const {
        T k_ = T(1) - (eta * eta * (T(1) - Dot(n) * Dot(n)));
        if (k_ < T(0)) {
            return TVec3(0);
        }
        return (*this * eta) - (n * (eta * Dot(n) + std::sqrt(k_)));
    }
};

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> operator*(T val, TVec3<T> vec) {
    return vec * val;
}

template <Numeric T>
[[nodiscard]] constexpr T Dot(TVec3<T> vecA, TVec3<T> vecB) {
    return vecA.Dot(vecB);
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Cross(TVec3<T> vecA, TVec3<T> vecB) {
    return vecA.Cross(vecB);
}

template <Numeric T>
[[nodiscard]] constexpr T LengthSquared(TVec3<T> vec) {
    return vec.LengthSquared();
}

template <Numeric T>
[[nodiscard]] T Length(TVec3<T> vec) {
    return vec.Length();
}

template <Numeric T>
[[nodiscard]] T Distance(TVec3<T> vecA, TVec3<T> vecB) {
    return (vecA - vecB).Length();
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Lerp(TVec3<T> vecA, TVec3<T> vecB, T time) {
    return vecA + (vecB - vecA) * time;
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Min(TVec3<T> vecA, TVec3<T> vecB) {
    return {Min(vecA.x, vecB.x), Min(vecA.y, vecB.y), Min(vecA.z, vecB.z)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Max(TVec3<T> vecA, TVec3<T> vecB) {
    return {Max(vecA.x, vecB.x), Max(vecA.y, vecB.y), Max(vecA.z, vecB.z)};
}

template <Numeric T>
[[nodiscard]] constexpr TVec3<T> Clamp(TVec3<T> vec, TVec3<T> lo, TVec3<T> hi) {
    return {Clamp(vec.x, lo.x, hi.x), Clamp(vec.y, lo.y, hi.y),
            Clamp(vec.z, lo.z, hi.z)};
}

using Vec3 = TVec3<float>;
using DVec3 = TVec3<double>;
using IVec3 = TVec3<int>;
using UVec3 = TVec3<unsigned>;
using BVec3 = TVec3<bool>;

}  // namespace GE::Math
