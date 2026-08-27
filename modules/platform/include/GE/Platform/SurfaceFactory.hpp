#pragma once

#include <cstdint>
#include <expected>
#include <memory>
#include <vector>

#include "Surface.hpp"
#include "Window.hpp"

namespace GE::Platform {
class SurfaceFactory {
public:
    virtual ~SurfaceFactory() = default;

    virtual std::vector<const char*> RequiredInstanceExtensions() = 0;

    virtual uint64_t InstanceHandle() { return 0; }

    virtual std::expected<std::unique_ptr<Surface>, Surface::Error>
    CreateSurface(Window& window, uint64_t instanceHandle) = 0;
};
}  // namespace GE::Platform
