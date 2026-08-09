#pragma once

/// GE::Math public API.
///
/// All glm usage is confined to the static library's translation units
/// (modules/math/src/*.cpp); these headers never include glm, so consumers of
/// `goiaba_math` have no glm dependency and can use every type below with
/// compile-time / inlineable semantics. Complex operations (matrix inverse,
/// quaternion slerp, projection builders, ...) are declared here and defined
/// out-of-line via glm.

#include "GE/Math/Common.hpp"
#include "GE/Math/Constants.hpp"
#include "GE/Math/Functions.hpp"
#include "GE/Math/Matrix.hpp"
#include "GE/Math/Quaternion.hpp"
#include "GE/Math/Vector2.hpp"
#include "GE/Math/Vector3.hpp"
#include "GE/Math/Vector4.hpp"