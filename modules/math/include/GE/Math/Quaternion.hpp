#pragma once

#include <cmath>
#include <type_traits>

#include "GE/Math/Common.hpp"
#include "GE/Math/Matrix.hpp"

namespace GE::Math {

/// Quaternion storing `x, y, z, w` — matching glm 1.0.1's default storage order
/// (GLM_FORCE_QUAT_DATA_WXYZ would switch to w-first; we do NOT define it).
/// Layout-compatible with `glm::qua<T>`, so the private bridge bit_casts
/// freely. Identity is (0, 0, 0, 1).
template <Numeric T>
struct TQuat {
    static_assert(std::is_floating_point_v<T>);
    using value_type = T;

    T x{};
    T y{};
    T z{};
    T w{T(1)};  // identity default

    constexpr TQuat() = default;
    constexpr TQuat(T x_, T y_, T z_, T w_) : x(x_), y(y_), z(z_), w(w_) {}
    template <Numeric U>
    explicit constexpr TQuat(const TQuat<U>& o)
        : x(static_cast<T>(o.x)),
          y(static_cast<T>(o.y)),
          z(static_cast<T>(o.z)),
          w(static_cast<T>(o.w)) {}

    [[nodiscard]] constexpr TQuat operator+() const { return *this; }
    [[nodiscard]] constexpr TQuat operator-() const { return {-x, -y, -z, -w}; }
    [[nodiscard]] constexpr TQuat operator+(TQuat r) const {
        return {x + r.x, y + r.y, z + r.z, w + r.w};
    }
    [[nodiscard]] constexpr TQuat operator-(TQuat r) const {
        return {x - r.x, y - r.y, z - r.z, w - r.w};
    }
    [[nodiscard]] constexpr TQuat operator*(T s) const {
        return {x * s, y * s, z * s, w * s};
    }
    [[nodiscard]] constexpr TQuat operator/(T s) const {
        return {x / s, y / s, z / s, w / s};
    }
    constexpr TQuat& operator+=(TQuat r) {
        x += r.x;
        y += r.y;
        z += r.z;
        w += r.w;
        return *this;
    }
    constexpr TQuat& operator-=(TQuat r) {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        w -= r.w;
        return *this;
    }
    constexpr TQuat& operator*=(T s) {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }

    [[nodiscard]] friend constexpr bool operator==(TQuat a, TQuat b) {
        return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
    }

    [[nodiscard]] constexpr T Dot(TQuat r) const {
        return x * r.x + y * r.y + z * r.z + w * r.w;
    }
    [[nodiscard]] constexpr T LengthSquared() const { return Dot(*this); }
    [[nodiscard]] constexpr TQuat Conjugate() const { return {-x, -y, -z, w}; }

    [[nodiscard]] T Length() const { return std::sqrt(LengthSquared()); }
    [[nodiscard]] TQuat Normalized() const {
        T len = Length();
        return len == T(0) ? *this : *this / len;
    }
    [[nodiscard]] constexpr TQuat Inverse() const {
        return Conjugate() / LengthSquared();
    }

    // -- Complex ops: declared here, defined in src/Quaternion.cpp via glm
    // ----- Hamilton product. this * rhs.
    [[nodiscard]] TQuat operator*(TQuat rhs) const;
    // Rotate a 3-vector by this quaternion.
    [[nodiscard]] TVec3<T> operator*(TVec3<T> v) const { return Rotate(v); }
    [[nodiscard]] TVec3<T> Rotate(TVec3<T> v) const;

    [[nodiscard]] static TQuat FromAxisAngle(TVec3<T> axis, T angleRadians);
    [[nodiscard]] static TQuat FromEulerAngles(TVec3<T> eulerRadians);
    [[nodiscard]] static TQuat FromMat3(const TMat<3, 3, T>& m);
    [[nodiscard]] static TQuat FromMat4(const TMat<4, 4, T>& m);
    [[nodiscard]] TMat<3, 3, T> ToMat3() const;
    [[nodiscard]] TMat<4, 4, T> ToMat4() const;
    [[nodiscard]] T Angle() const;
    [[nodiscard]] TVec3<T> Axis() const;
};

template <Numeric T>
[[nodiscard]] constexpr TQuat<T> operator*(T s, TQuat<T> q) {
    return q * s;
}

template <Numeric T>
[[nodiscard]] constexpr T Dot(TQuat<T> a, TQuat<T> b) {
    return a.Dot(b);
}

template <Numeric T>
[[nodiscard]] constexpr T LengthSquared(TQuat<T> q) {
    return q.LengthSquared();
}

template <Numeric T>
[[nodiscard]] T Length(TQuat<T> q) {
    return q.Length();
}

/// Spherical linear interpolation between two quaternions.
template <Numeric T>
[[nodiscard]] TQuat<T> Slerp(TQuat<T> a, TQuat<T> b, T t);

/// Normalized linear interpolation (cheaper than Slerp, often good enough).
template <Numeric T>
[[nodiscard]] TQuat<T> Nlerp(TQuat<T> a, TQuat<T> b, T t) {
    return (a + (b - a) * t).Normalized();
}

template <Numeric T>
[[nodiscard]] TQuat<T> Lerp(TQuat<T> a, TQuat<T> b, T t) {
    return a + (b - a) * t;
}

using Quat = TQuat<float>;
using DQuat = TQuat<double>;

}  // namespace GE::Math