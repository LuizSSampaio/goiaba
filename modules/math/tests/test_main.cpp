#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

// The math module's public API must be usable through this single umbrella
// header with zero glm presence. Including it here proves consumers don't need
// glm headers to compile against `goiaba_math`.
#include "GE/Math.hpp"

// ---- Compile-time proof (no runtime, exercises constexpr paths) -----------
static_assert(GE::Math::Vec3{1, 2, 3}.Dot({4, 5, 6}) == 32.0f);
static_assert(GE::Math::Vec3{1, 2, 3}.Cross({4, 5, 6}) ==
              GE::Math::Vec3{-3, 6, -3});
static_assert(GE::Math::Vec4{1, 2, 3, 4}.LengthSquared() == 30.0f);
static_assert(GE::Math::Min(2.0f, 3.0f) == 2.0f);
static_assert(GE::Math::Clamp(5.0f, 0.0f, 4.0f) == 4.0f);
static_assert(GE::Math::Radians(180.0f) == GE::Math::PiF);
static_assert(GE::Math::Mat4::Identity()[0][0] == 1.0f);
static_assert(GE::Math::Mat4::Identity()[1][0] == 0.0f);

TEST_CASE("Static assertions compiled") {
    CHECK(true);  // the real checks are the static_asserts above
}