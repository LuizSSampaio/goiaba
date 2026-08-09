#pragma once

#include <numbers>
namespace GE::Math {

/// Mathematical constants as variable templates.
template <typename T>
inline constexpr T Pi = T(std::numbers::pi);

template <typename T>
inline constexpr T Tau = Pi<T> * T(2);

template <typename T>
inline constexpr T HalfPi = Pi<T> * T(0.5);

template <typename T>
inline constexpr T TwoOverPi = T(2) / Pi<T>;

template <typename T>
inline constexpr T RootTwo = T(std::numbers::sqrt2);

template <typename T>
inline constexpr T Epsilon = T(1.0e-5);

inline constexpr float PiF = Pi<float>;
inline constexpr float EpsilonF = Epsilon<float>;

}  // namespace GE::Math
