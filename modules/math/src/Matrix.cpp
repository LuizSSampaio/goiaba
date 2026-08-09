#include <GE/Math/Matrix.hpp>
#include <glm/matrix.hpp>  // glm::inverse, glm::determinant, operator*

#include "GlmBridge.hpp"

namespace GE::Math {

template <int C, int R, Numeric T>
TMat<C, R, T> TMat<C, R, T>::Inverse() const
    requires(C == R)
{
    return detail::FromGlm<TMat<C, R, T>>(
        glm::inverse(detail::ToGlm<glm::mat<C, R, T>>(*this)));
}

template <int C, int R, Numeric T>
T TMat<C, R, T>::Determinant() const
    requires(C == R)
{
    return glm::determinant(detail::ToGlm<glm::mat<C, R, T>>(*this));
}

template <int C, int R, Numeric T>
template <int C2>
TMat<C2, R, T> TMat<C, R, T>::operator*(const TMat<C2, C, T>& rhs) const {
    return detail::FromGlm<TMat<C2, R, T>>(
        detail::ToGlm<glm::mat<C, R, T>>(*this) *
        detail::ToGlm<glm::mat<C2, C, T>>(rhs));
}

template <int C, int R, Numeric T>
TVec<R, T> TMat<C, R, T>::operator*(const TVec<C, T>& rhs) const {
    return detail::FromGlm<TVec<R, T>>(detail::ToGlm<glm::mat<C, R, T>>(*this) *
                                       detail::ToGlm<glm::vec<C, T>>(rhs));
}

}  // namespace GE::Math

// ---------------------------------------------------------------------------
// Explicit instantiations. Supported: square float and double matrices (the
// shapes glm's inverses are defined for). Non-square / unsupported instantions
// intentionally not provided (link error if used — add on demand).
// ---------------------------------------------------------------------------

// 2x2
template struct GE::Math::TMat<2, 2, float>;
template GE::Math::TMat<2, 2, float> GE::Math::TMat<2, 2, float>::operator*
    <2>(const GE::Math::TMat<2, 2, float>&) const;
// 3x3
template struct GE::Math::TMat<3, 3, float>;
template GE::Math::TMat<3, 3, float> GE::Math::TMat<3, 3, float>::operator*
    <3>(const GE::Math::TMat<3, 3, float>&) const;
// 4x4
template struct GE::Math::TMat<4, 4, float>;
template GE::Math::TMat<4, 4, float> GE::Math::TMat<4, 4, float>::operator*
    <4>(const GE::Math::TMat<4, 4, float>&) const;
// 2x2 double
template struct GE::Math::TMat<2, 2, double>;
template GE::Math::TMat<2, 2, double> GE::Math::TMat<2, 2, double>::operator*
    <2>(const GE::Math::TMat<2, 2, double>&) const;
// 3x3 double
template struct GE::Math::TMat<3, 3, double>;
template GE::Math::TMat<3, 3, double> GE::Math::TMat<3, 3, double>::operator*
    <3>(const GE::Math::TMat<3, 3, double>&) const;
// 4x4 double
template struct GE::Math::TMat<4, 4, double>;
template GE::Math::TMat<4, 4, double> GE::Math::TMat<4, 4, double>::operator*
    <4>(const GE::Math::TMat<4, 4, double>&) const;
