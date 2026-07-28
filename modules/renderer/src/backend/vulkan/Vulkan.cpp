#include "Vulkan.hpp"

#include <GE/Logger.hpp>

#include "vulkan/vulkan.hpp"

using namespace GE::Render::Backend;

Vulkan::~Vulkan() { this->instance_.destroy(); }

std::expected<void, Vulkan::Error> Vulkan::Init(
    const std::string& appName, const std::string& engineName,
    const Vulkan::Extensions& extensions) {
    auto instanceRes = this->CreateInstance(appName, engineName, extensions);
    if (!instanceRes) {
        Logger::Critical(
            "Vulkan Backend failed creating a instance with code: " +
                std::to_string(static_cast<uint32_t>(instanceRes.error())),
            Logger::Engine);
        return std::unexpected(Error::FailedInstanceCreation);
    }

    return {};
}

std::expected<void, vk::Result> Vulkan::CreateInstance(
    const std::string& appName, const std::string& engineName,
    const Extensions& extensions) {
    vk::ApplicationInfo appInfo{
        .pApplicationName = appName.c_str(),
        .pEngineName = engineName.c_str(),
        .apiVersion = VKApiVersion,
    };

    vk::InstanceCreateInfo instanceCI{
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = extensions.count,
        .ppEnabledExtensionNames = extensions.names};

    vk::Instance instance;
    vk::Result res = vk::createInstance(&instanceCI, nullptr, &instance);

    if (res != vk::Result::eSuccess) {
        return std::unexpected(res);
    }
    this->instance_ = instance;
    return {};
}
