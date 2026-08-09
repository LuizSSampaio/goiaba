#include <GE/Math/Quaternion.hpp>
#include <glm/ext/quaternion_common.hpp>         // glm::slerp
#include <glm/ext/quaternion_trigonometric.hpp>  // angleAxis, angle, axis
#include <glm/gtc/quaternion.hpp>  // mat3_cast, mat4_cast, quat_cast

#include "GlmBridge.hpp"

namespace GE::Math {

template <Numeric T>
TQuat<T> TQuat<T>::operator*(TQuat rhs) const {
    return detail::FromGlm<TQuat<T>>(detail::ToGlm<glm::qua<T>>(*this) *
                                     detail::ToGlm<glm::qua<T>>(rhs));
}

template <Numeric T>
TVec3<T> TQuat<T>::Rotate(TVec3<T> vec) const {
    return detail::FromGlm<TVec3<T>>(detail::ToGlm<glm::qua<T>>(*this) *
                                     detail::ToGlm<glm::vec<3, T>>(vec));
}

template <Numeric T>
TQuat<T> TQuat<T>::FromAxisAngle(TVec3<T> axis, T angleRadians) {
    return detail::FromGlm<TQuat<T>>(
        glm::angleAxis(angleRadians, detail::ToGlm<glm::vec<3, T>>(axis)));
}

template <Numeric T>
TQuat<T> TQuat<T>::FromEulerAngles(TVec3<T> eulerRadians) {
    return detail::FromGlm<TQuat<T>>(
        glm::qua<T>(detail::ToGlm<glm::vec<3, T>>(eulerRadians)));
}

template <Numeric T>
TQuat<T> TQuat<T>::FromMat3(const TMat<3, 3, T>& mat) {
    return detail::FromGlm<TQuat<T>>(
        glm::quat_cast(detail::ToGlm<glm::mat<3, 3, T>>(mat)));
}

template <Numeric T>
TQuat<T> TQuat<T>::FromMat4(const TMat<4, 4, T>& mat) {
    return detail::FromGlm<TQuat<T>>(
        glm::quat_cast(detail::ToGlm<glm::mat<4, 4, T>>(mat)));
}

template <Numeric T>
TMat<3, 3, T> TQuat<T>::ToMat3() const {
    return detail::FromGlm<TMat<3, 3, T>>(
        glm::mat3_cast(detail::ToGlm<glm::qua<T>>(*this)));
}

template <Numeric T>
TMat<4, 4, T> TQuat<T>::ToMat4() const {
    return detail::FromGlm<TMat<4, 4, T>>(
        glm::mat4_cast(detail::ToGlm<glm::qua<T>>(*this)));
}

template <Numeric T>
T TQuat<T>::Angle() const {
    return glm::angle(detail::ToGlm<glm::qua<T>>(*this));
}

template <Numeric T>
TVec3<T> TQuat<T>::Axis() const {
    return detail::FromGlm<TVec3<T>>(
        glm::axis(detail::ToGlm<glm::qua<T>>(*this)));
}

template <Numeric T>
TQuat<T> Slerp(TQuat<T> quatA, TQuat<T> quatB, T time) {
    return detail::FromGlm<TQuat<T>>(glm::slerp(
        detail::ToGlm<glm::qua<T>>(quatA), detail::ToGlm<glm::qua<T>>(quatB), time));
}

}  // namespace GE::Math

template struct GE::Math::TQuat<float>;
template struct GE::Math::TQuat<double>;

template GE::Math::Quat GE::Math::Slerp<float>(GE::Math::Quat, GE::Math::Quat,
                                               float);
template GE::Math::DQuat GE::Math::Slerp<double>(GE::Math::DQuat,
                                                 GE::Math::DQuat, double);
