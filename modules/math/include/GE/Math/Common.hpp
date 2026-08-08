#pragma once

#include <GE/Math/Constants.hpp>
#include <concepts>
#include <type_traits>

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
[[nodiscard]] constexpr T Abs(T val) {
    return val < T(0) ? -val : val;
}

template <Numeric T>
[[nodiscard]] constexpr T Min(T valA, T valB) {
    return valA < valB ? valA : valB;
}

template <Numeric T>
[[nodiscard]] constexpr T Max(T valA, T valB) {
    return valA > valB ? valA : valB;
}

template <Numeric T>
[[nodiscard]] constexpr T Clamp(T val, T lo, T hi) {
    const auto valHi = (val > hi ? hi : val);
    return val < lo ? lo : valHi;
}

/// Linear interpolation: a + t*(b - a). `t` is not clamped.
template <Numeric T>
[[nodiscard]] constexpr T Lerp(T start, T end, T time) {
    return start + (time * (end - start));
}

template <typename T>
[[nodiscard]] constexpr T Step(T edge, T val) {
    return val < edge ? T(0) : T(1);
}

template <typename T>
[[nodiscard]] constexpr T Smoothstep(T edge0, T edge1, T val) {
    T t = Clamp((val - edge0) / (edge1 - edge0), T(0), T(1));
    return t * t * (T(3) - T(2) * t);
}

template <typename T>
[[nodiscard]] constexpr T Radians(T degrees) {
    constexpr auto defautDegree = 180;
    return degrees * (Pi<T> / T(defautDegree));
}

template <typename T>
[[nodiscard]] constexpr T Degrees(T radians) {
    constexpr auto defautDegree = 180;
    return radians * (T(defautDegree) / Pi<T>);
}

template <Numeric T>
[[nodiscard]] constexpr bool Approximately(T valA, T valB, T eps = Epsilon<T>) {
    return Abs(valA - valB) <= eps * Max(T(1), Max(Abs(valA), Abs(valB)));
}

}  // namespace GE::Math
