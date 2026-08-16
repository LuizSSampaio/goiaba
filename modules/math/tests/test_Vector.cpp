#include <doctest/doctest.h>

#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "GE/Math.hpp"

using namespace GE::Math;

TEST_CASE("Vec3 arithmetic") {
    Vec3 a{1, 2, 3};
    Vec3 b{4, 5, 6};
    CHECK(a + b == Vec3{5, 7, 9});
    CHECK(b - a == Vec3{3, 3, 3});
    CHECK(-a == Vec3{-1, -2, -3});
    CHECK(a * 2.0f == Vec3{2, 4, 6});
    CHECK(2.0f * a == Vec3{2, 4, 6});
    Vec3 c = a;
    c += b;
    CHECK(c == Vec3{5, 7, 9});
}

TEST_CASE("Vec3 dot and cross") {
    Vec3 a{1, 0, 0};
    Vec3 b{0, 1, 0};
    CHECK(a.Dot(b) == 0.0f);
    CHECK(a.Cross(b) == Vec3{0, 0, 1});
    CHECK(Vec3{1, 2, 3}.Dot({4, 5, 6}) == 32.0f);
}

TEST_CASE("Vec3 length/normalize vs glm") {
    Vec3 v{3, 4, 0};
    glm::vec3 g(v.x, v.y, v.z);
    CHECK(v.LengthSquared() == 25.0f);
    CHECK(v.Length() == doctest::Approx(glm::length(g)));
    Vec3 n = v.Normalized();
    glm::vec3 gn = glm::normalize(g);
    CHECK(n.x == doctest::Approx(gn.x));
    CHECK(n.y == doctest::Approx(gn.y));
    CHECK(n.z == doctest::Approx(gn.z));
    CHECK(n.Length() == doctest::Approx(1.0f).epsilon(1e-4));
}

TEST_CASE("Vec3 zero normalize is safe") {
    CHECK(Vec3{0, 0, 0}.Normalized() == Vec3{0, 0, 0});
}

TEST_CASE("Vec2 and Vec4 component access") {
    Vec2 v2{1, 2};
    CHECK(v2[0] == 1.0f);
    CHECK(v2[1] == 2.0f);
    Vec4 v4{1, 2, 3, 4};
    CHECK(v4.Xyz() == Vec3{1, 2, 3});
}

TEST_CASE("Vec4 from Vec3") {
    Vec4 v = Vec4{Vec3{1, 2, 3}, 4.0f};
    CHECK(v == Vec4{1, 2, 3, 4});
}

TEST_CASE("Integer vector aliases") {
    IVec3 i{1, 2, 3};
    CHECK(i + IVec3{4, 5, 6} == IVec3{5, 7, 9});
    UVec2 u{1, 2};
    CHECK(u * 2u == UVec2{2, 4});
}

TEST_CASE("BVec aliases exist and compare") {
    GE::Math::BVec3 b{true, false, true};
    CHECK(b == GE::Math::BVec3{true, false, true});
    CHECK(b != GE::Math::BVec3{false, false, true});
}

TEST_CASE("Reflect") {
    Vec3 d{1, -1, 0};
    Vec3 n{0, 1, 0};
    CHECK(d.Reflect(n) == Vec3{1, 1, 0});
}

TEST_CASE("Lerp and Smoothstep") {
    CHECK(Lerp(Vec3{0, 0, 0}, Vec3{10, 20, 30}, 0.5f) == Vec3{5, 10, 15});
    CHECK(Smoothstep(0.0f, 1.0f, 0.5f) == doctest::Approx(0.5f));
}