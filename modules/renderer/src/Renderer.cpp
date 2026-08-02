#define VMA_IMPLEMENTATION
#include "GE/Renderer.hpp"

#include <SDL3/SDL_vulkan.h>

#include <GE/Logger.hpp>
#include <string>

#include "./backend/vulkan/Vulkan.hpp"

using namespace GE::Render;

std::expected<void, Renderer::Error> Renderer::Init(Renderer::Backend backend) {
    switch (backend) {
        case Renderer::Backend::Vulkan:
            auto vkRes = this->InitVulkan();
            if (!vkRes.has_value()) {
                return std::unexpected(vkRes.error());
            }
            break;
    }

    return {};
}

std::expected<void, Renderer::Error> Renderer::InitVulkan() {
    GE::Render::Backend::Vulkan backend = GE::Render::Backend::Vulkan();
    GE::Render::Backend::Vulkan::Extensions extensions;
    extensions.names = SDL_Vulkan_GetInstanceExtensions(&extensions.count);

    auto backendRes = backend.Init("Sample", "Goiaba", extensions);
    if (!backendRes.has_value()) {
        Logger::Critical("Failed to initialize Vulkan Backend(Error: " +
                             std::to_string(backendRes.error()) + ")",
                         Logger::Engine);
        return std::unexpected(Renderer::Error::FailedToInitializeBackend);
    }

    // TODO: Create window and surface

    return {};
}

void Renderer::Run() {}
