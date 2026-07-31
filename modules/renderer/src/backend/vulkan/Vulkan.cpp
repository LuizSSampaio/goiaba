#include "Vulkan.hpp"

#include <GE/Logger.hpp>
#include <algorithm>
#include <cstdint>
#include <expected>
#include <map>
#include <utility>

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
        return std::unexpected(instanceRes.error());
    }

    return {};
}

std::expected<void, Vulkan::Error> Vulkan::CreateInstance(
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
        return std::unexpected(Vulkan::Error::FailedInstanceCreation);
    }
    this->instance_ = instance;
    return {};
}

std::expected<void, Vulkan::Error> Vulkan::SelectDevice() {
    vk::PhysicalDevice physicalDevice = nullptr;
    auto physicalDevices = this->instance_.enumeratePhysicalDevices();
    if (physicalDevices.empty()) {
        Logger::Critical("Failed to find a GPU with Vulkan support",
                         Logger::Engine);
        return std::unexpected(Vulkan::Error::NoVulkanDevice);
    }

    auto bestDevice = Vulkan::PickBestDevice(physicalDevices);
    if (!bestDevice.has_value()) {
        return std::unexpected(bestDevice.error());
    }
    physicalDevice = bestDevice.value();

    return {};
}

std::expected<vk::PhysicalDevice, Vulkan::Error> Vulkan::PickBestDevice(
    const std::vector<vk::PhysicalDevice>& devices) {
    std::multimap<int, vk::PhysicalDevice> candidates;
    for (const auto& device : devices) {
        uint32_t score = 0;

        auto deviceProperties = device.getProperties();
        if (deviceProperties.deviceType ==
            vk::PhysicalDeviceType::eDiscreteGpu) {
            constexpr int discreteGpuScore = 1000;
            score += discreteGpuScore;
        }

        score += deviceProperties.limits.maxImageDimension2D;

        auto deviceFeatures = device.getFeatures();
        if (!static_cast<bool>(deviceFeatures.geometryShader)) {
            continue;
        }

        auto deviceQueueFamilies = device.getQueueFamilyProperties();
        bool supportsGraphics = std::ranges::any_of(
            deviceQueueFamilies, [](auto const& queueFamilyProperties) {
                return static_cast<bool>(queueFamilyProperties.queueFlags &
                                         vk::QueueFlagBits::eGraphics);
            });
        if (!supportsGraphics) {
            continue;
        }

        candidates.insert(std::make_pair(score, device));
    }

    if (candidates.empty() || candidates.rbegin()->first <= 0) {
        return std::unexpected(Vulkan::Error::NoSuitableDevice);
    }

    return candidates.rbegin()->second;
}
