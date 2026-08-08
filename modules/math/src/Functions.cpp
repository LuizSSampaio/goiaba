#include "GE/Math/Functions.hpp"

#include <glm/ext/matrix_clip_space.hpp>  // perspectiveRH_ZO, orthoRH_ZO
#include <glm/ext/matrix_transform.hpp>   // lookAtRH, translate, rotate, scale

#include "GlmBridge.hpp"

namespace GE::Math {

template <Numeric T>
TMat<4, 4, T> Perspective(T fovyRadians, T aspect, T nearZ, T farZ) {
    return detail::FromGlm<TMat<4, 4, T>>(
        glm::perspectiveRH_ZO(fovyRadians, aspect, nearZ, farZ));
}

template <Numeric T>
TMat<4, 4, T> Orthographic(T left, T right, T bottom, T top, T nearZ, T farZ) {
    return detail::FromGlm<TMat<4, 4, T>>(
        glm::orthoRH_ZO(left, right, bottom, top, nearZ, farZ));
}

template <Numeric T>
TMat<4, 4, T> LookAt(TVec3<T> eye, TVec3<T> center, TVec3<T> up) {
    return detail::FromGlm<TMat<4, 4, T>>(
        glm::lookAtRH(detail::ToGlm<glm::vec<3, T>>(eye),
                      detail::ToGlm<glm::vec<3, T>>(center),
                      detail::ToGlm<glm::vec<3, T>>(up)));
}

template <Numeric T>
TMat<4, 4, T> Translate(TMat<4, 4, T> m, TVec3<T> v) {
    return detail::FromGlm<TMat<4, 4, T>>(glm::translate(
        detail::ToGlm<glm::mat<4, 4, T>>(m), detail::ToGlm<glm::vec<3, T>>(v)));
}

template <Numeric T>
TMat<4, 4, T> Scale(TMat<4, 4, T> m, TVec3<T> v) {
    return detail::FromGlm<TMat<4, 4, T>>(glm::scale(
        detail::ToGlm<glm::mat<4, 4, T>>(m), detail::ToGlm<glm::vec<3, T>>(v)));
}

template <Numeric T>
TMat<4, 4, T> Rotate(TMat<4, 4, T> m, T angleRadians, TVec3<T> axis) {
    return detail::FromGlm<TMat<4, 4, T>>(
        glm::rotate(detail::ToGlm<glm::mat<4, 4, T>>(m), angleRadians,
                    detail::ToGlm<glm::vec<3, T>>(axis)));
}

}  // namespace GE::Math

template GE::Math::Mat4 GE::Math::Perspective<float>(float, float, float,
                                                     float);
template GE::Math::Mat4 GE::Math::Orthographic<float>(float, float, float,
                                                      float, float, float);
template GE::Math::Mat4 GE::Math::LookAt<float>(GE::Math::Vec3, GE::Math::Vec3,
                                                GE::Math::Vec3);
template GE::Math::Mat4 GE::Math::Translate<float>(GE::Math::Mat4,
                                                   GE::Math::Vec3);
template GE::Math::Mat4 GE::Math::Scale<float>(GE::Math::Mat4, GE::Math::Vec3);
template GE::Math::Mat4 GE::Math::Rotate<float>(GE::Math::Mat4, float,
                                                GE::Math::Vec3);

template GE::Math::DMat4 GE::Math::Perspective<double>(double, double, double,
                                                       double);
template GE::Math::DMat4 GE::Math::Orthographic<double>(double, double, double,
                                                        double, double, double);
template GE::Math::DMat4 GE::Math::LookAt<double>(GE::Math::DVec3,
                                                  GE::Math::DVec3,
                                                  GE::Math::DVec3);
template GE::Math::DMat4 GE::Math::Translate<double>(GE::Math::DMat4,
                                                     GE::Math::DVec3);
template GE::Math::DMat4 GE::Math::Scale<double>(GE::Math::DMat4,
                                                 GE::Math::DVec3);
template GE::Math::DMat4 GE::Math::Rotate<double>(GE::Math::DMat4, double,
                                                  GE::Math::DVec3);