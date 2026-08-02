#pragma once

#include <cstdint>
#include <expected>

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
    ~Renderer() = default;

    std::expected<void, Error> Init(Backend backend);
    void Run();

private:
    std::expected<void, Error> InitVulkan();
};
}  // namespace GE::Render
