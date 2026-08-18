#pragma once

#include <GE/Platform/SurfaceFactory.hpp>
#include <GE/Platform/Window.hpp>
#include <cstdint>
#include <expected>
#include <memory>

#include "GE/Backend.hpp"

namespace GE::Render {
class Renderer {
public:
    enum Error : uint8_t {
        FailedToInitializeBackend,
    };

    enum Backend : uint8_t {
        Vulkan,
    };

    Renderer() = default;
    ~Renderer();

    std::expected<void, Error> Init(
        Backend backend, std::unique_ptr<GE::Platform::Window> window,
        std::unique_ptr<GE::Platform::SurfaceFactory> surfaceFactory);
    void Run();

private:
    std::unique_ptr<IBackend> backend_;
    std::unique_ptr<GE::Platform::Window> window_;

    std::expected<void, Error> InitVulkan(
        std::unique_ptr<GE::Platform::SurfaceFactory> surfaceFactory);
};
}  // namespace GE::Render
