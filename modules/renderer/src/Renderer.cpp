#include <expected>
#include <memory>

#define VMA_IMPLEMENTATION

#include <GE/Logger.hpp>
#include <GE/Platform/SurfaceFactory.hpp>
#include <GE/Platform/Window.hpp>
#include <string>

#include "./backends/Vulkan/Vulkan.hpp"
#include "GE/Renderer.hpp"

using namespace GE::Render;

Renderer::~Renderer() {
    this->backend_.reset();
    this->window_.reset();
}

std::expected<void, Renderer::Error> Renderer::Init(
    Renderer::Backend backend, std::unique_ptr<GE::Platform::Window> window,
    std::unique_ptr<GE::Platform::SurfaceFactory> surfaceFactory) {
    if (!window || !surfaceFactory) {
        return std::unexpected(Renderer::Error::FailedToInitializeBackend);
    }

    this->window_ = std::move(window);
    switch (backend) {
        case Renderer::Backend::Vulkan:
            auto vkRes = this->InitVulkan(std::move(surfaceFactory));
            if (!vkRes.has_value()) {
                return std::unexpected(vkRes.error());
            }
            break;
    }

    return {};
}

std::expected<void, Renderer::Error> Renderer::InitVulkan(
    std::unique_ptr<GE::Platform::SurfaceFactory> surfaceFactory) {
    auto backend = std::make_unique<Backends::Vulkan>();

    auto extensions = Backends::Vulkan::Extensions(
        surfaceFactory->RequiredInstanceExtensions());

    auto backendRes = backend->Init(this->window_, surfaceFactory, "Sample",
                                    "Goiaba", extensions);
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
    this->window_->PollEvents();

    this->backend_->RenderPass();
}
