#include <GE/Renderer.hpp>

#include "GE/Logger.hpp"

int main() {
    GE::Logger::Init();
    GE::Render::Renderer renderer = GE::Render::Renderer();
    auto res = renderer.Init(GE::Render::Renderer::Vulkan);
    if (!res.has_value()) {
        GE::Logger::Shutdown();
        return 1;
    }

    renderer.Run();

    GE::Logger::Shutdown();
    return 0;
}
