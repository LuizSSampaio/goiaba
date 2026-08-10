#include <doctest/doctest.h>
#include <cmath>

#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

#include "GE/Math.hpp"

using namespace GE::Math;

static bool eqApprox(GE::Math::Vec3 a, glm::vec3 b, float eps = 1e-4f) {
    return std::abs(a.x - b.x) <= eps && std::abs(a.y - b.y) <= eps &&
           std::abs(a.z - b.z) <= eps;
}

TEST_CASE("FromAxisAngle rotates known vector (right-handed)") {
    Quat q = Quat::FromAxisAngle(Vec3{0, 1, 0}, Radians(90.0f));
    Vec3 r = q.Rotate(Vec3{1, 0, 0});
    // Behaviourally proves the x,y,z,w bit_cast order agreement with glm:
    CHECK(r.x == doctest::Approx(0.0f).epsilon(1e-4));
    CHECK(r.y == doctest::Approx(0.0f).epsilon(1e-4));
    CHECK(r.z == doctest::Approx(-1.0f).epsilon(1e-4));
}

TEST_CASE("Quat rotate matches glm::angleAxis") {
    Quat q = Quat::FromAxisAngle(Vec3{0, 1, 0}, Radians(30.0f));
    Vec3 v = q.Rotate(Vec3{1, 0, 0});
    glm::quat gq = glm::angleAxis(glm::radians(30.0f), glm::vec3(0, 1, 0));
    glm::vec3 gv = gq * glm::vec3(1, 0, 0);
    CHECK(eqApprox(v, gv));
}

TEST_CASE("Quaternion multiplication vs glm") {
    Quat a = Quat::FromAxisAngle(Vec3{1, 0, 0}, Radians(10.0f));
    Quat b = Quat::FromAxisAngle(Vec3{0, 1, 0}, Radians(20.0f));
    Quat r = a * b;
    glm::quat ga = glm::angleAxis(glm::radians(10.0f), glm::vec3(1, 0, 0));
    glm::quat gb = glm::angleAxis(glm::radians(20.0f), glm::vec3(0, 1, 0));
    glm::quat gr = ga * gb;
    CHECK(r.x == doctest::Approx(gr.x).epsilon(1e-4));
    CHECK(r.y == doctest::Approx(gr.y).epsilon(1e-4));
    CHECK(r.z == doctest::Approx(gr.z).epsilon(1e-4));
    CHECK(r.w == doctest::Approx(gr.w).epsilon(1e-4));
}

TEST_CASE("ToMat3/ToMat4 vs glm casts") {
    Quat q = Quat::FromAxisAngle(Vec3{0, 1, 0}, Radians(45.0f));
    glm::quat gq = glm::angleAxis(glm::radians(45.0f), glm::vec3(0, 1, 0));
    Mat3 m3 = q.ToMat3();
    glm::mat3 gm3 = glm::mat3_cast(gq);
    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            CHECK(m3[c][r] == doctest::Approx(gm3[c][r]).epsilon(1e-4));

    Mat4 m4 = q.ToMat4();
    glm::mat4 gm4 = glm::mat4_cast(gq);
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            CHECK(m4[c][r] == doctest::Approx(gm4[c][r]).epsilon(1e-4));
}

TEST_CASE("FromMat3 round-trips (up to sign)") {
    Quat q = Quat::FromAxisAngle(Vec3{0, 0, 1}, Radians(60.0f));
    Mat3 m = q.ToMat3();
    Quat q2 = Quat::FromMat3(m);
    glm::quat gq = glm::quat_cast(glm::mat3_cast(
        glm::angleAxis(glm::radians(60.0f), glm::vec3(0, 0, 1))));
    CHECK(q2.x == doctest::Approx(gq.x).epsilon(1e-4));
    CHECK(q2.y == doctest::Approx(gq.y).epsilon(1e-4));
    CHECK(q2.z == doctest::Approx(gq.z).epsilon(1e-4));
    CHECK(q2.w == doctest::Approx(gq.w).epsilon(1e-4));
}

TEST_CASE("Slerp midpoint == glm slerp") {
    Quat a = Quat::FromAxisAngle(Vec3{1, 0, 0}, Radians(0.0f));
    Quat b = Quat::FromAxisAngle(Vec3{1, 0, 0}, Radians(90.0f));
    Quat s = Slerp(a, b, 0.5f);
    glm::quat ga(1, 0, 0, 0);  // identity
    glm::quat gb = glm::angleAxis(glm::radians(90.0f), glm::vec3(1, 0, 0));
    glm::quat gs = glm::slerp(ga, gb, 0.5f);
    CHECK(s.x == doctest::Approx(gs.x).epsilon(1e-4));
    CHECK(s.y == doctest::Approx(gs.y).epsilon(1e-4));
    CHECK(s.z == doctest::Approx(gs.z).epsilon(1e-4));
    CHECK(s.w == doctest::Approx(gs.w).epsilon(1e-4));
}

TEST_CASE("Angle/axis") {
    Quat q = Quat::FromAxisAngle(Vec3{0, 1, 0}, Radians(45.0f));
    CHECK(q.Angle() == doctest::Approx(Radians(45.0f)).epsilon(1e-4));
    Vec3 ax = q.Axis();
    CHECK(ax.x == doctest::Approx(0.0f).epsilon(1e-4));
    CHECK(ax.y == doctest::Approx(1.0f).epsilon(1e-4));
    CHECK(ax.z == doctest::Approx(0.0f).epsilon(1e-4));
}