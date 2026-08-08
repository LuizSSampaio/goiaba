#pragma once

#include <cstddef>
#include <type_traits>

#include "GE/Math/Common.hpp"
#include "GE/Math/Vector2.hpp"
#include "GE/Math/Vector3.hpp"
#include "GE/Math/Vector4.hpp"

namespace GE::Math {

/// Column-major matrix: `C` columns, `R` rows, each column a `TVec<R, T>`.
/// Layout-compatible with `glm::mat<C, R, T>` (which stores `vec<R,T>
/// value[C]`), so the private bridge can bit_cast between them at zero cost.
///
/// Simple ops are constexpr in this header. Complex ops (`Inverse`,
/// `Determinant`, matrix*matrix, matrix*vector) are declared here but defined
/// out-of-line via glm in `src/Matrix.cpp`; only explicitly instantiated
/// (float/double square) types are linkable.
template <int C, int R, Numeric T>
struct TMat {
    static_assert(C >= 2 && C <= 4 && R >= 2 && R <= 4);
    static_assert(std::is_floating_point_v<T>,
                  "Complex ops require floating-point T");

    using ColType = TVec<R, T>;
    using value_type = T;
    static constexpr int kCols = C;
    static constexpr int kRows = R;

    ColType cols[C]{};

    constexpr TMat() = default;

    [[nodiscard]] constexpr ColType& operator[](int col) { return cols[col]; }
    [[nodiscard]] constexpr const ColType& operator[](int col) const {
        return cols[col];
    }

    [[nodiscard]] static constexpr TMat Zero() { return TMat{}; }

    [[nodiscard]] static constexpr TMat Identity()
        requires(C == R)
    {
        TMat m{};
        for (int i = 0; i < C; ++i) m.cols[i][i] = T(1);
        return m;
    }

    /// Diagonal matrix from a vector of length min(C, R).
    [[nodiscard]] static constexpr TMat Diagonal(TVec<(C < R ? C : R), T> d) {
        TMat m{};
        constexpr int n = C < R ? C : R;
        for (int i = 0; i < n; ++i) m.cols[i][i] = d[i];
        return m;
    }

    [[nodiscard]] constexpr TMat operator+() const { return *this; }
    [[nodiscard]] constexpr TMat operator-() const {
        TMat r{};
        for (int i = 0; i < C; ++i) r.cols[i] = -cols[i];
        return r;
    }

    [[nodiscard]] constexpr TMat operator+(TMat rhs) const {
        TMat r{};
        for (int i = 0; i < C; ++i) r.cols[i] = cols[i] + rhs.cols[i];
        return r;
    }
    [[nodiscard]] constexpr TMat operator-(TMat rhs) const {
        TMat r{};
        for (int i = 0; i < C; ++i) r.cols[i] = cols[i] - rhs.cols[i];
        return r;
    }
    [[nodiscard]] constexpr TMat operator*(T s) const {
        TMat r{};
        for (int i = 0; i < C; ++i) r.cols[i] = cols[i] * s;
        return r;
    }
    [[nodiscard]] constexpr TMat operator/(T s) const {
        TMat r{};
        for (int i = 0; i < C; ++i) r.cols[i] = cols[i] / s;
        return r;
    }

    constexpr TMat& operator+=(TMat rhs) {
        for (int i = 0; i < C; ++i) cols[i] += rhs.cols[i];
        return *this;
    }
    constexpr TMat& operator-=(TMat rhs) {
        for (int i = 0; i < C; ++i) cols[i] -= rhs.cols[i];
        return *this;
    }
    constexpr TMat& operator*=(T s) {
        for (int i = 0; i < C; ++i) cols[i] *= s;
        return *this;
    }
    constexpr TMat& operator/=(T s) {
        for (int i = 0; i < C; ++i) cols[i] /= s;
        return *this;
    }

    [[nodiscard]] friend constexpr bool operator==(TMat a, TMat b) {
        for (int i = 0; i < C; ++i)
            if (a.cols[i] != b.cols[i]) return false;
        return true;
    }

    /// Transpose (pure permutation) — constexpr, no glm needed.
    [[nodiscard]] constexpr TMat<R, C, T> Transpose() const {
        TMat<R, C, T> r{};
        for (int i = 0; i < C; ++i)
            for (int j = 0; j < R; ++j) r.cols[j][i] = cols[i][j];
        return r;
    }

    // -- Complex ops: declared here, defined in src/Matrix.cpp via glm -------
    [[nodiscard]] TMat Inverse() const
        requires(C == R);
    [[nodiscard]] T Determinant() const
        requires(C == R);

    /// this (C cols x R rows) * rhs (C2 cols x C rows) -> (C2 cols x R rows)
    template <int C2>
    [[nodiscard]] TMat<C2, R, T> operator*(const TMat<C2, C, T>& rhs) const;
    /// this (C x R) * vec (C) -> vec (R)
    [[nodiscard]] TVec<R, T> operator*(const TVec<C, T>& rhs) const;
};

template <int C, int R, Numeric T>
[[nodiscard]] constexpr TMat<C, R, T> operator*(T s, TMat<C, R, T> m) {
    return m * s;
}

using Mat2 = TMat<2, 2, float>;
using Mat2x3 = TMat<2, 3, float>;
using Mat3x2 = TMat<3, 2, float>;
using Mat2x4 = TMat<2, 4, float>;
using Mat4x2 = TMat<4, 2, float>;
using Mat3 = TMat<3, 3, float>;
using Mat3x4 = TMat<3, 4, float>;
using Mat4x3 = TMat<4, 3, float>;
using Mat4 = TMat<4, 4, float>;

using DMat2 = TMat<2, 2, double>;
using DMat3 = TMat<3, 3, double>;
using DMat4 = TMat<4, 4, double>;

}  // namespace GE::Math