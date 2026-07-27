#include "GE/Renderer.hpp"

#include <SDL3/SDL_vulkan.h>

#include <cstdint>

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

GE::Renderer::Renderer() {
    vk::ApplicationInfo appInfo{
        .pApplicationName = "Test",
        .apiVersion = vk::ApiVersion13,
    };

    uint32_t instanceExtensionsCount = 0;
    char const* const* instaceExtensions =
        SDL_Vulkan_GetInstanceExtensions(&instanceExtensionsCount);

    vk::InstanceCreateInfo instanceCI{
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = instanceExtensionsCount,
        .ppEnabledExtensionNames = instaceExtensions,
    };

    vk::Instance instance;
    vk::createInstance(&instanceCI, nullptr, &instance);
}

GE::Renderer::~Renderer() {}

void GE::Renderer::Run() {}
