#include <expected>
#include <memory>

#include "src/SDLWindow.hpp"
#define VMA_IMPLEMENTATION
#include <SDL3/SDL_vulkan.h>

#include <GE/Logger.hpp>
#include <string>

#include "./backends/Vulkan.hpp"
#include "GE/Renderer.hpp"

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
    GE::Render::Backends::Vulkan backend = GE::Render::Backends::Vulkan();
    GE::Render::Backends::Vulkan::Extensions extensions;
    extensions.names = SDL_Vulkan_GetInstanceExtensions(&extensions.count);

    auto sdlWindow = std::make_shared<SDLWindow>();
    this->window_ = sdlWindow;
    auto winRes = this->window_->InitWindow("Sample", 1920, 1080,
                                            Window::Flag::Resizable);
    if (!winRes.has_value()) {
        Logger::Critical("Failed to create window(Error: " +
                             std::to_string(winRes.error()) + ")",
                         Logger::Engine);
        return std::unexpected(Renderer::Error::FailedToCreateWindow);
    }

    auto backendRes = backend.Init(sdlWindow, "Sample", "Goiaba", extensions);
    if (!backendRes.has_value()) {
        Logger::Critical("Failed to initialize Vulkan Backend(Error: " +
                             std::to_string(backendRes.error()) + ")",
                         Logger::Engine);
        return std::unexpected(Renderer::Error::FailedToInitializeBackend);
    }

    return {};
}

void Renderer::Run() {}

std::shared_ptr<Window> Renderer::window() { return this->window_; }
