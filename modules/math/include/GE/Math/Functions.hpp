#pragma once

#include <GE/Math/Common.hpp>
#include <GE/Math/Matrix.hpp>
#include <GE/Math/Vector3.hpp>

namespace GE::Math {

/// Perspective projection. Vulkan clip space (depth 0..1), right-handed —
/// enforced privately in the library's build via GLM_FORCE_DEPTH_ZERO_TO_ONE.
/// `fovyRadians` is the full vertical field of view in radians.
template <Numeric T>
[[nodiscard]] TMat<4, 4, T> Perspective(T fovyRadians, T aspect, T nearZ,
                                        T farZ);

/// Orthographic projection, Vulkan clip space (depth 0..1), right-handed.
template <Numeric T>
[[nodiscard]] TMat<4, 4, T> Orthographic(T left, T right, T bottom, T top,
                                         T nearZ, T farZ);

/// Right-handed look-at view matrix (eye -> target, with `up`).
template <Numeric T>
[[nodiscard]] TMat<4, 4, T> LookAt(TVec3<T> eye, TVec3<T> center, TVec3<T> up);

/// Translate a (column-major) 4x4 matrix by `v`.
template <Numeric T>
[[nodiscard]] TMat<4, 4, T> Translate(TMat<4, 4, T> mat, TVec3<T> vec);

/// Scale a (column-major) 4x4 matrix by `v`.
template <Numeric T>
[[nodiscard]] TMat<4, 4, T> Scale(TMat<4, 4, T> mat, TVec3<T> vec);

/// Rotate a (column-major) 4x4 matrix by `angleRadians` around `axis`.
template <Numeric T>
[[nodiscard]] TMat<4, 4, T> Rotate(TMat<4, 4, T> mat, T angleRadians,
                                   TVec3<T> axis);

}  // namespace GE::Math
