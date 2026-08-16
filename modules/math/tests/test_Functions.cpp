#include <doctest/doctest.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/vec3.hpp>

#include "GE/Math.hpp"

using namespace GE::Math;

TEST_CASE("Perspective: near maps to 0, far to 1 (Vulkan clip space)") {
    Mat4 p = Perspective(Radians(90.0f), 1.0f, 1.0f, 100.0f);
    glm::mat4 gp =
        glm::perspectiveRH_ZO(glm::radians(90.0f), 1.0f, 1.0f, 100.0f);
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            CHECK(p[c][r] == doctest::Approx(gp[c][r]).epsilon(1e-4));

    // A point at -near (looking -Z, RH) maps to z_clip = 0.
    Vec4 nearH = p * Vec4{0, 0, -1, 1};
    CHECK(nearH.z == doctest::Approx(0.0f).epsilon(1e-4));
    CHECK(nearH.w == doctest::Approx(1.0f).epsilon(1e-4));
    // A point at -far maps to z_clip = far (pre-divide); after /w -> 1.
    Vec4 farH = p * Vec4{0, 0, -100, 1};
    CHECK(farH.z / farH.w == doctest::Approx(1.0f).epsilon(1e-3));
}

TEST_CASE("Orthographic vs glm RH_ZO") {
    Mat4 o = Orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
    glm::mat4 go = glm::orthoRH_ZO(-10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 100.0f);
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            CHECK(o[c][r] == doctest::Approx(go[c][r]).epsilon(1e-4));
}

TEST_CASE("LookAt vs glm lookAtRH") {
    Mat4 v = LookAt(Vec3{0, 0, 5}, Vec3{0, 0, 0}, Vec3{0, 1, 0});
    glm::mat4 gv = glm::lookAtRH(glm::vec3(0, 0, 5), glm::vec3(0, 0, 0),
                                 glm::vec3(0, 1, 0));
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            CHECK(v[c][r] == doctest::Approx(gv[c][r]).epsilon(1e-4));
}

TEST_CASE("Translate / Scale / Rotate vs glm") {
    Mat4 t = Translate(Mat4::Identity(), Vec3{1, 2, 3});
    glm::mat4 gt = glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3));
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            CHECK(t[c][r] == doctest::Approx(gt[c][r]).epsilon(1e-4));

    Mat4 s = Scale(Mat4::Identity(), Vec3{2, 3, 4});
    glm::mat4 gs = glm::scale(glm::mat4(1.0f), glm::vec3(2, 3, 4));
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            CHECK(s[c][r] == doctest::Approx(gs[c][r]).epsilon(1e-4));

    Mat4 r = Rotate(Mat4::Identity(), Radians(30.0f), Vec3{0, 1, 0});
    glm::mat4 gr =
        glm::rotate(glm::mat4(1.0f), glm::radians(30.0f), glm::vec3(0, 1, 0));
    for (int c = 0; c < 4; ++c)
        for (int r0 = 0; r0 < 4; ++r0)
            CHECK(r[c][r0] == doctest::Approx(gr[c][r0]).epsilon(1e-4));
}