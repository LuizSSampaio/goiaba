#pragma once

namespace GE::Math {

/// Mathematical constants as variable templates.
template <typename T>
inline constexpr T Pi = T(3.141592653589793238462643383279502884);

template <typename T>
inline constexpr T Tau = Pi<T> * T(2);

template <typename T>
inline constexpr T HalfPi = Pi<T> * T(0.5);

template <typename T>
inline constexpr T TwoOverPi = T(2) / Pi<T>;

template <typename T>
inline constexpr T RootTwo = T(1.414213562373095048801688724209698079);

template <typename T>
inline constexpr T Epsilon = T(1.0e-5);

inline constexpr float PiF = Pi<float>;
inline constexpr float EpsilonF = Epsilon<float>;

}  // namespace GE::Math