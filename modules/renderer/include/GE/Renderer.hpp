#pragma once

#include <GE/Window.hpp>
#include <cstdint>
#include <expected>
#include <memory>

namespace GE::Render {
class Renderer {
public:
    enum Error : uint8_t {
        FailedToCreateWindow,
        FailedToInitializeBackend,
    };

    enum Backend : uint8_t {
        Vulkan,
    };

    Renderer() = default;
    ~Renderer() = default;

    std::expected<void, Error> Init(Backend backend);
    void Run();

    std::shared_ptr<Window> window();

private:
    std::shared_ptr<Window> window_;

    std::expected<void, Error> InitVulkan();
};
}  // namespace GE::Render
