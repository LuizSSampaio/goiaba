#pragma once

#include <GE/Math/Common.hpp>
#include <GE/Math/Matrix.hpp>
#include <cmath>
#include <type_traits>

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
    explicit constexpr TQuat(const TQuat<U>& o_)
        : x(static_cast<T>(o_.x)),
          y(static_cast<T>(o_.y)),
          z(static_cast<T>(o_.z)),
          w(static_cast<T>(o_.w)) {}

    [[nodiscard]] constexpr TQuat operator+() const { return *this; }
    [[nodiscard]] constexpr TQuat operator-() const { return {-x, -y, -z, -w}; }
    [[nodiscard]] constexpr TQuat operator+(TQuat rhs) const {
        return {x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w};
    }
    [[nodiscard]] constexpr TQuat operator-(TQuat rhs) const {
        return {x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w};
    }
    [[nodiscard]] constexpr TQuat operator*(T val) const {
        return {x * val, y * val, z * val, w * val};
    }
    [[nodiscard]] constexpr TQuat operator/(T val) const {
        return {x / val, y / val, z / val, w / val};
    }
    constexpr TQuat& operator+=(TQuat rhs) {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        w += rhs.w;
        return *this;
    }
    constexpr TQuat& operator-=(TQuat rhs) {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        w -= rhs.w;
        return *this;
    }
    constexpr TQuat& operator*=(T val) {
        x *= val;
        y *= val;
        z *= val;
        w *= val;
        return *this;
    }

    [[nodiscard]] friend constexpr bool operator==(TQuat quatA, TQuat quatB) {
        return quatA.x == quatB.x && quatA.y == quatB.y && quatA.z == quatB.z &&
               quatA.w == quatB.w;
    }

    [[nodiscard]] constexpr T Dot(TQuat rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
    }
    [[nodiscard]] constexpr T LengthSquared() const { return Dot(*this); }
    [[nodiscard]] constexpr TQuat Conjugate() const { return {-x, -y, -z, w}; }

    [[nodiscard]] T Length() const { return std::sqrt(LengthSquared()); }
    [[nodiscard]] TQuat Normalized() const {
        T len = Length();
        return len == T(0) ? *this : *this / len;
    }
[[nodiscard]] constexpr TQuat Inverse() const {
    const T len2 = LengthSquared();
    return len2 == T(0) ? *this : (Conjugate() / len2);
}

    // -- Complex ops: declared here, defined in src/Quaternion.cpp via glm
    // ----- Hamilton product. this * rhs.
    [[nodiscard]] TQuat operator*(TQuat rhs) const;
    // Rotate a 3-vector by this quaternion.
    [[nodiscard]] TVec3<T> operator*(TVec3<T> vec) const { return Rotate(vec); }
    [[nodiscard]] TVec3<T> Rotate(TVec3<T> vec) const;

    [[nodiscard]] static TQuat FromAxisAngle(TVec3<T> axis, T angleRadians);
    [[nodiscard]] static TQuat FromEulerAngles(TVec3<T> eulerRadians);
    [[nodiscard]] static TQuat FromMat3(const TMat<3, 3, T>& mat);
    [[nodiscard]] static TQuat FromMat4(const TMat<4, 4, T>& mat);
    [[nodiscard]] TMat<3, 3, T> ToMat3() const;
    [[nodiscard]] TMat<4, 4, T> ToMat4() const;
    [[nodiscard]] T Angle() const;
    [[nodiscard]] TVec3<T> Axis() const;
};

template <Numeric T>
[[nodiscard]] constexpr TQuat<T> operator*(T val, TQuat<T> quat) {
    return quat * val;
}

template <Numeric T>
[[nodiscard]] constexpr T Dot(TQuat<T> quatA, TQuat<T> quatB) {
    return quatA.Dot(quatB);
}

template <Numeric T>
[[nodiscard]] constexpr T LengthSquared(TQuat<T> quat) {
    return quat.LengthSquared();
}

template <Numeric T>
[[nodiscard]] T Length(TQuat<T> quat) {
    return quat.Length();
}

/// Spherical linear interpolation between two quaternions.
template <Numeric T>
[[nodiscard]] TQuat<T> Slerp(TQuat<T> quatA, TQuat<T> quatB, T time);

/// Normalized linear interpolation (cheaper than Slerp, often good enough).
template <Numeric T>
[[nodiscard]] TQuat<T> Nlerp(TQuat<T> quatA, TQuat<T> quatB, T time) {
    return (quatA + (quatB - quatA) * time).Normalized();
}

template <Numeric T>
[[nodiscard]] TQuat<T> Lerp(TQuat<T> quatA, TQuat<T> quatB, T time) {
    return quatA + (quatB - quatA) * time;
}

using Quat = TQuat<float>;
using DQuat = TQuat<double>;

}  // namespace GE::Math
