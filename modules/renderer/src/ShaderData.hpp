#pragma once

#include <GE/Math.hpp>
#include <array>
#include <cstdint>

struct ShaderData {
    GE::Math::Mat4 projection;
    GE::Math::Mat4 view;
    std::array<GE::Math::Mat4, 3> model;
    // TODO: Remove hardcoded light position
    GE::Math::Vec4 lightPos = {0.0f, -10.0f, 10.0f, 0.0f};
    uint32_t selected = 1;
};
