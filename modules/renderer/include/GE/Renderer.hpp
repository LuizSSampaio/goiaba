#pragma once

#include <GE/Window.hpp>
#include <cstdint>
#include <expected>
#include <memory>

#include "GE/Backend.hpp"

namespace GE::Render {
class Renderer {
public:
    enum Error : uint8_t {
        FailedToInitializeSDL,
        FailedToCreateWindow,
        FailedToInitializeBackend,
    };

    enum Backend : uint8_t {
        Vulkan,
    };

    Renderer() = default;
    ~Renderer();

    std::expected<void, Error> Init(Backend backend);
    void Run();

    std::shared_ptr<Window> window();

private:
    std::unique_ptr<IBackend> backend_;
    std::shared_ptr<Window> window_;

    std::expected<void, Error> InitVulkan();
};
}  // namespace GE::Render
