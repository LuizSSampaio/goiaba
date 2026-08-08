#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

#include "GE/Math/Constants.hpp"

namespace GE::Math {

/// Numeric component types (excludes `bool`, which only supports relational
/// ops). Vectors/matrices are templated on this concept.
template <typename T>
concept Numeric = std::is_arithmetic_v<T> && !std::same_as<T, bool>;

/// Any arithmetic component, including `bool` (used so `BVec*` aliases can
/// exist; arithmetic ops on bool vectors fail to compile only if used).
template <typename T>
concept Component = std::is_arithmetic_v<T>;

// Forward declarations so the generic `TVec<N, T>` alias below can name
// TVec2/3/4 before their full definitions (in Vector2/3/4.hpp).
template <Component T>
struct TVec2;
template <Component T>
struct TVec3;
template <Component T>
struct TVec4;

namespace detail {
template <int N, typename T>
struct VecDispatch;
template <typename T>
struct VecDispatch<2, T> {
    using type = TVec2<T>;
};
template <typename T>
struct VecDispatch<3, T> {
    using type = TVec3<T>;
};
template <typename T>
struct VecDispatch<4, T> {
    using type = TVec4<T>;
};
}  // namespace detail

/// Generic vector alias: `TVec<3, float>` resolves to `TVec3<float>`.
template <int N, typename T>
using TVec = typename detail::VecDispatch<N, T>::type;

/// Absolute value, constexpr-friendly (no <cmath> dependency).
template <Numeric T>
[[nodiscard]] constexpr T Abs(T x) {
    return x < T(0) ? -x : x;
}

template <Numeric T>
[[nodiscard]] constexpr T Min(T a, T b) {
    return a < b ? a : b;
}

template <Numeric T>
[[nodiscard]] constexpr T Max(T a, T b) {
    return a > b ? a : b;
}

template <Numeric T>
[[nodiscard]] constexpr T Clamp(T x, T lo, T hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

/// Linear interpolation: a + t*(b - a). `t` is not clamped.
template <Numeric T>
[[nodiscard]] constexpr T Lerp(T a, T b, T t) {
    return a + t * (b - a);
}

template <typename T>
[[nodiscard]] constexpr T Step(T edge, T x) {
    return x < edge ? T(0) : T(1);
}

template <typename T>
[[nodiscard]] constexpr T Smoothstep(T edge0, T edge1, T x) {
    T t = Clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
    return t * t * (T(3) - T(2) * t);
}

template <typename T>
[[nodiscard]] constexpr T Radians(T degrees) {
    return degrees * (Pi<T> / T(180));
}

template <typename T>
[[nodiscard]] constexpr T Degrees(T radians) {
    return radians * (T(180) / Pi<T>);
}

template <Numeric T>
[[nodiscard]] constexpr bool Approximately(T a, T b, T eps = Epsilon<T>) {
    return Abs(a - b) <= eps * Max(T(1), Max(Abs(a), Abs(b)));
}

}  // namespace GE::Math