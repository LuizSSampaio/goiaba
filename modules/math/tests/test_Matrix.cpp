#include <doctest/doctest.h>

#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/vec4.hpp>

#include "GE/Math.hpp"

using namespace GE::Math;

static Mat4 MakeSeq(int base) {
    Mat4 m{};
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r) m[c][r] = base * 0.1f + c * 7 + r;
    return m;
}

static glm::mat4 ToGlmRaw(const Mat4& m) {
    return glm::mat4(m[0][0], m[0][1], m[0][2], m[0][3], m[1][0], m[1][1],
                     m[1][2], m[1][3], m[2][0], m[2][1], m[2][2], m[2][3],
                     m[3][0], m[3][1], m[3][2], m[3][3]);
}

TEST_CASE("Identity and Zero") {
    Mat4 I = Mat4::Identity();
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r) CHECK(I[c][r] == (c == r ? 1.0f : 0.0f));
    CHECK(Mat4::Zero()[0][0] == 0.0f);
}

TEST_CASE("Transpose") {
    Mat4 m = MakeSeq(0);
    Mat4 t = m.Transpose();
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r) CHECK(t[c][r] == m[r][c]);
}

TEST_CASE("mat * mat == glm") {
    Mat4 a = MakeSeq(1);
    Mat4 b = MakeSeq(3);
    Mat4 r = a * b;
    glm::mat4 gr = ToGlmRaw(a) * ToGlmRaw(b);
    for (int c = 0; c < 4; ++c)
        for (int row = 0; row < 4; ++row)
            CHECK(r[c][row] == doctest::Approx(gr[c][row]).epsilon(1e-4));
}

TEST_CASE("mat * vec == glm") {
    Mat4 m = MakeSeq(2);
    Vec4 v{1, 2, 3, 4};
    Vec4 r = m * v;
    glm::vec4 gr = ToGlmRaw(m) * glm::vec4(1, 2, 3, 4);
    CHECK(r.x == doctest::Approx(gr.x).epsilon(1e-4));
    CHECK(r.y == doctest::Approx(gr.y).epsilon(1e-4));
    CHECK(r.z == doctest::Approx(gr.z).epsilon(1e-4));
    CHECK(r.w == doctest::Approx(gr.w).epsilon(1e-4));
}

TEST_CASE("Determinant and inverse vs glm; M * M^-1 = I") {
    Mat4 m{};
    m[0] = Vec4{2, 0, 0, 0};
    m[1] = Vec4{0, 3, 0, 0};
    m[2] = Vec4{0, 0, 4, 0};
    m[3] = Vec4{1, 1, 1, 1};
    glm::mat4 gm = ToGlmRaw(m);

    CHECK(m.Determinant() == doctest::Approx(glm::determinant(gm)));
    Mat4 inv = m.Inverse();
    glm::mat4 ginv = glm::inverse(gm);
    for (int c = 0; c < 4; ++c)
        for (int row = 0; row < 4; ++row)
            CHECK(inv[c][row] == doctest::Approx(ginv[c][row]).epsilon(1e-4));

    Mat4 id = m * inv;
    for (int c = 0; c < 4; ++c)
        for (int row = 0; row < 4; ++row)
            CHECK(id[c][row] ==
                  doctest::Approx(c == row ? 1.0f : 0.0f).epsilon(1e-4));
}

TEST_CASE("Mat3 and Mat2 inverse") {
    Mat3 m{};
    m[0] = Vec3{2, 1, 0};
    m[1] = Vec3{0, 3, 0};
    m[2] = Vec3{0, 0, 5};
    Mat3 id = m * m.Inverse();
    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            CHECK(id[c][r] ==
                  doctest::Approx(c == r ? 1.0f : 0.0f).epsilon(1e-4));

    Mat2 m2{};
    m2[0] = {4, 0};
    m2[1] = {1, 2};
    CHECK(m2.Determinant() == doctest::Approx(8.0f));
    Mat2 id2 = m2 * m2.Inverse();
    CHECK(id2[0][0] == doctest::Approx(1.0f).epsilon(1e-4));
    CHECK(id2[1][1] == doctest::Approx(1.0f).epsilon(1e-4));
}