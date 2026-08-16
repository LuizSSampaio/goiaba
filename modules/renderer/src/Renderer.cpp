#include <cstdint>
#include <expected>
#include <memory>

#include "src/SDLWindow.hpp"
#define VMA_IMPLEMENTATION
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <GE/Logger.hpp>
#include <string>

#include "./backends/Vulkan/Vulkan.hpp"
#include "GE/Renderer.hpp"

using namespace GE::Render;

Renderer::~Renderer() {
    this->backend_.reset();
    this->window_.reset();
    SDL_Quit();
}

std::expected<void, Renderer::Error> Renderer::Init(Renderer::Backend backend) {
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        return std::unexpected(Renderer::Error::FailedToInitializeSDL);
    }

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
    auto backend = std::make_unique<Backends::Vulkan>();

    uint32_t count = 0;
    const char* const* names = SDL_Vulkan_GetInstanceExtensions(&count);
    auto extensions = Backends::Vulkan::Extensions(names, count);

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

    auto backendRes = backend->Init(sdlWindow, "Sample", "Goiaba", extensions);
    if (!backendRes.has_value()) {
        Logger::Critical("Failed to initialize Vulkan Backend(Error: " +
                             std::to_string(backendRes.error()) + ")",
                         Logger::Engine);
        return std::unexpected(Renderer::Error::FailedToInitializeBackend);
    }

    this->backend_ = std::move(backend);
    return {};
}

void Renderer::Run() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
    }

    this->backend_->RenderPass();
}

std::shared_ptr<Window> Renderer::window() { return this->window_; }
