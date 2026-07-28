#include "GE/Renderer.hpp"

#include <SDL3/SDL_vulkan.h>

#include "./backend/vulkan/Vulkan.hpp"

using namespace GE::Render;

Renderer::Renderer() {
    Backend::Vulkan backend = Backend::Vulkan();
    Backend::Vulkan::Extensions extensions;
    extensions.names = SDL_Vulkan_GetInstanceExtensions(&extensions.count);
    backend.Init("Sample", "Goiaba", extensions);
}

Renderer::~Renderer() {}

void Renderer::Run() {}
