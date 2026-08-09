#pragma once

// PRIVATE header — never included from public headers (modules/math/include).
// This is the ONLY place glm is included in the math module. It provides
// zero-cost bit_cast converters between GE::Math types and glm types plus
// compile-time layout-compatibility guards.

#include <GE/Math/Matrix.hpp>
#include <GE/Math/Quaternion.hpp>
#include <GE/Math/Vector2.hpp>
#include <GE/Math/Vector3.hpp>
#include <GE/Math/Vector4.hpp>
#include <bit>
#include <cstddef>
#include <type_traits>

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

// Template guards: each returns `true` (after its inner static_asserts pass),
// so a `static_assert(Guard<...>())` at namespace scope forces instantiation
// and surfaces a hard compile error on any layout mismatch. Ge is the GE::Math
// type; Gm is the glm type it must be bit_cast-compatible with.

// Vectors: trivially copyable + standard-layout on our side.
template <typename Ge, typename Gm>
constexpr bool AssertVecLayout() {
    static_assert(sizeof(Ge) == sizeof(Gm));
    static_assert(alignof(Ge) == alignof(Gm));
    static_assert(std::is_trivially_copyable_v<Ge>);
    static_assert(std::is_standard_layout_v<Ge>);
    return true;
}

// Matrices: trivially copyable (offsetof-free checks; no standard-layout req).
template <typename Ge, typename Gm>
constexpr bool AssertMatLayout() {
    static_assert(sizeof(Ge) == sizeof(Gm));
    static_assert(alignof(Ge) == alignof(Gm));
    static_assert(std::is_trivially_copyable_v<Ge>);
    return true;
}

// Quaternions: trivially copyable.
template <typename Ge, typename Gm>
constexpr bool AssertQuatLayout() {
    static_assert(sizeof(Ge) == sizeof(Gm));
    static_assert(alignof(Ge) == alignof(Gm));
    static_assert(std::is_trivially_copyable_v<Ge>);
    return true;
}

static_assert(AssertVecLayout<TVec2<float>, glm::vec2>());
static_assert(AssertVecLayout<TVec3<float>, glm::vec3>());
static_assert(AssertVecLayout<TVec4<float>, glm::vec4>());
static_assert(AssertVecLayout<TVec2<double>, glm::dvec2>());
static_assert(AssertVecLayout<TVec3<double>, glm::dvec3>());
static_assert(AssertVecLayout<TVec4<double>, glm::dvec4>());

static_assert(AssertMatLayout<TMat<2, 2, float>, glm::mat<2, 2, float>>());
static_assert(AssertMatLayout<TMat<3, 2, float>, glm::mat<3, 2, float>>());
static_assert(AssertMatLayout<TMat<2, 3, float>, glm::mat<2, 3, float>>());
static_assert(AssertMatLayout<TMat<4, 2, float>, glm::mat<4, 2, float>>());
static_assert(AssertMatLayout<TMat<2, 4, float>, glm::mat<2, 4, float>>());
static_assert(AssertMatLayout<TMat<3, 3, float>, glm::mat<3, 3, float>>());
static_assert(AssertMatLayout<TMat<4, 3, float>, glm::mat<4, 3, float>>());
static_assert(AssertMatLayout<TMat<3, 4, float>, glm::mat<3, 4, float>>());
static_assert(AssertMatLayout<TMat<4, 4, float>, glm::mat<4, 4, float>>());
static_assert(AssertMatLayout<TMat<2, 2, double>, glm::mat<2, 2, double>>());
static_assert(AssertMatLayout<TMat<3, 3, double>, glm::mat<3, 3, double>>());
static_assert(AssertMatLayout<TMat<4, 4, double>, glm::mat<4, 4, double>>());

static_assert(AssertQuatLayout<TQuat<float>, glm::qua<float>>());
static_assert(AssertQuatLayout<TQuat<double>, glm::qua<double>>());

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
[[nodiscard]] inline G ToGlm(const E& mathEl) noexcept {
    static_assert(sizeof(G) == sizeof(E) && alignof(G) == alignof(E));
    static_assert(std::is_trivially_copyable_v<G> &&
                  std::is_trivially_copyable_v<E>);
    return std::bit_cast<G>(mathEl);
}

template <typename E, typename G>
[[nodiscard]] inline E FromGlm(const G& glmEl) noexcept {
    static_assert(sizeof(G) == sizeof(E) && alignof(G) == alignof(E));
    static_assert(std::is_trivially_copyable_v<G> &&
                  std::is_trivially_copyable_v<E>);
    return std::bit_cast<E>(glmEl);
}

}  // namespace GE::Math::detail
