#pragma once

#include <cstdint>

namespace GE::Platform {
class Surface {
public:
    enum class Error : uint8_t { FailedToCreateSurface };

    virtual ~Surface() = default;

    [[nodiscard]] virtual uint64_t nativeHandle() const = 0;
};
}  // namespace GE::Platform
