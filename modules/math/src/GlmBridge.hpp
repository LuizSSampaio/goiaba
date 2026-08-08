#pragma once

// PRIVATE header — never included from public headers (modules/math/include).
// This is the ONLY place glm is included in the math module. It provides
// zero-cost bit_cast converters between GE::Math types and glm types plus
// compile-time layout-compatibility guards.

#include <bit>
#include <cstddef>
#include <type_traits>

#include "GE/Math/Matrix.hpp"
#include "GE/Math/Quaternion.hpp"
#include "GE/Math/Vector2.hpp"
#include "GE/Math/Vector3.hpp"
#include "GE/Math/Vector4.hpp"

// glm type headers (granular).
#include <glm/ext/quaternion_double.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat2x3.hpp>
#include <glm/mat2x4.hpp>
#include <glm/mat3x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x2.hpp>
#include <glm/mat4x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace GE::Math::detail {

// ---------------------------------------------------------------------------
// Layout-compatibility guards. If any of these fail, the bit_cast bridge would
// be unsound (e.g. a GLM_FORCE_* alignment/order macro was defined). They make
// such misconfiguration a hard, obvious compile error.
// ---------------------------------------------------------------------------

// Trivially copyable + standard-layout on our side (glm side only needs
// trivially-copyable + same size/alignment for bit_cast).
#define GE_MATH_ASSERT_VEC(elt, N, Ge)                       \
    static_assert(sizeof(Ge) == sizeof(glm::vec<N, elt>));   \
    static_assert(alignof(Ge) == alignof(glm::vec<N, elt>)); \
    static_assert(std::is_trivially_copyable_v<Ge>);         \
    static_assert(std::is_standard_layout_v<Ge>);

#define GE_MATH_ASSERT_MAT(C, R, elt)                                        \
    static_assert(sizeof(TMat<C, R, elt>) == sizeof(glm::mat<C, R, elt>));   \
    static_assert(alignof(TMat<C, R, elt>) == alignof(glm::mat<C, R, elt>)); \
    static_assert(std::is_trivially_copyable_v<TMat<C, R, elt>>);

#define GE_MATH_ASSERT_QUAT(elt)                                  \
    static_assert(sizeof(TQuat<elt>) == sizeof(glm::qua<elt>));   \
    static_assert(alignof(TQuat<elt>) == alignof(glm::qua<elt>)); \
    static_assert(std::is_trivially_copyable_v<TQuat<elt>>);

GE_MATH_ASSERT_VEC(float, 2, TVec2<float>)
GE_MATH_ASSERT_VEC(float, 3, TVec3<float>)
GE_MATH_ASSERT_VEC(float, 4, TVec4<float>)
GE_MATH_ASSERT_VEC(double, 2, TVec2<double>)
GE_MATH_ASSERT_VEC(double, 3, TVec3<double>)
GE_MATH_ASSERT_VEC(double, 4, TVec4<double>)

GE_MATH_ASSERT_MAT(2, 2, float)
GE_MATH_ASSERT_MAT(3, 2, float)
GE_MATH_ASSERT_MAT(2, 3, float)
GE_MATH_ASSERT_MAT(4, 2, float)
GE_MATH_ASSERT_MAT(2, 4, float)
GE_MATH_ASSERT_MAT(3, 3, float)
GE_MATH_ASSERT_MAT(4, 3, float)
GE_MATH_ASSERT_MAT(3, 4, float)
GE_MATH_ASSERT_MAT(4, 4, float)
GE_MATH_ASSERT_MAT(2, 2, double)
GE_MATH_ASSERT_MAT(3, 3, double)
GE_MATH_ASSERT_MAT(4, 4, double)

GE_MATH_ASSERT_QUAT(float)
GE_MATH_ASSERT_QUAT(double)

#undef GE_MATH_ASSERT_VEC
#undef GE_MATH_ASSERT_MAT
#undef GE_MATH_ASSERT_QUAT

// Member-order sanity (our side is standard-layout, so offsetof is safe).
static_assert(offsetof(TVec3<float>, z) == 2 * sizeof(float));
static_assert(offsetof(TVec4<float>, w) == 3 * sizeof(float));
static_assert(offsetof(TQuat<float>, x) == 0);
static_assert(offsetof(TQuat<float>, w) == 3 * sizeof(float));
using BridgeMat4f = TMat<4, 4, float>;  // alias to keep offsetof macro happy
static_assert(offsetof(BridgeMat4f, cols) == 0);  // column-major

// ---------------------------------------------------------------------------
// Zero-cost converters via std::bit_cast (compiles to nothing; layout verified
// above). ToGlm<glm::vec3>(ge); FromGlm<GE::Math::Vec3>(glmVec).
// ---------------------------------------------------------------------------
template <typename G, typename E>
[[nodiscard]] inline G ToGlm(const E& e) noexcept {
    static_assert(sizeof(G) == sizeof(E) && alignof(G) == alignof(E));
    static_assert(std::is_trivially_copyable_v<G> &&
                  std::is_trivially_copyable_v<E>);
    return std::bit_cast<G>(e);
}

template <typename E, typename G>
[[nodiscard]] inline E FromGlm(const G& g) noexcept {
    static_assert(sizeof(G) == sizeof(E) && alignof(G) == alignof(E));
    static_assert(std::is_trivially_copyable_v<G> &&
                  std::is_trivially_copyable_v<E>);
    return std::bit_cast<E>(g);
}

}  // namespace GE::Math::detail