#include "GE/Math/Matrix.hpp"

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
#define GE_MATH_INSTANTIATE_SQUARE(N, T)                                \
    template struct GE::Math::TMat<N, N, T>;                            \
    template GE::Math::TMat<N, N, T> GE::Math::TMat<N, N, T>::operator* \
        <N>(const GE::Math::TMat<N, N, T>&) const;

GE_MATH_INSTANTIATE_SQUARE(2, float)
GE_MATH_INSTANTIATE_SQUARE(3, float)
GE_MATH_INSTANTIATE_SQUARE(4, float)
GE_MATH_INSTANTIATE_SQUARE(2, double)
GE_MATH_INSTANTIATE_SQUARE(3, double)
GE_MATH_INSTANTIATE_SQUARE(4, double)

#undef GE_MATH_INSTANTIATE_SQUARE