#include "Vulkan.hpp"

#include <algorithm>
#include <cstdint>
#include <expected>
#include <iterator>
#include <utility>

using namespace GE::Render::Backend;

std::expected<void, Vulkan::Error> Vulkan::Init(
    const std::string& appName, const std::string& engineName,
    const Vulkan::Extensions& extensions) {
    auto instanceRes = this->CreateInstance(appName, engineName, extensions);
    if (!instanceRes) {
        return std::unexpected(instanceRes.error());
    }

    auto physicalDeviceRes = this->SelectPhysicalDevice();
    if (!physicalDeviceRes) {
        return std::unexpected(physicalDeviceRes.error());
    }

    auto queueAndDeviceRes =
        this->CreateQueueAndDevice(physicalDeviceRes.value());
    if (!queueAndDeviceRes) {
        return std::unexpected(queueAndDeviceRes.error());
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

    auto result = this->context_.createInstance(instanceCI, nullptr);
    if (!result.has_value()) {
        return std::unexpected(Vulkan::Error::FailedInstanceCreation);
    }

    this->instance_ = std::move(result.value());
    return {};
}

std::expected<vk::raii::PhysicalDevice, Vulkan::Error>
Vulkan::SelectPhysicalDevice() {
    auto enumResult = this->instance_.enumeratePhysicalDevices();
    if (!enumResult.has_value()) {
        return std::unexpected(Vulkan::Error::NoVulkanDevice);
    }
    auto physicalDevices = std::move(enumResult.value());
    if (physicalDevices.empty()) {
        return std::unexpected(Vulkan::Error::NoVulkanDevice);
    }

    std::optional<size_t> bestIndex;
    int bestScore = 0;

    for (size_t i = 0; i < physicalDevices.size(); i++) {
        int score = 0;

        auto deviceProperties = physicalDevices[i].getProperties();
        if (deviceProperties.deviceType ==
            vk::PhysicalDeviceType::eDiscreteGpu) {
            constexpr int discreteGpuScore = 1000;
            score += discreteGpuScore;
        }

        score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

        auto deviceFeatures = physicalDevices[i].getFeatures();
        if (!static_cast<bool>(deviceFeatures.geometryShader)) {
            continue;
        }

        auto deviceQueueFamilies =
            physicalDevices[i].getQueueFamilyProperties();
        bool supportsGraphics = std::ranges::any_of(
            deviceQueueFamilies, [](auto const& queueFamilyProperties) {
                return static_cast<bool>(queueFamilyProperties.queueFlags &
                                         vk::QueueFlagBits::eGraphics);
            });
        if (!supportsGraphics) {
            continue;
        }

        if (score > bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }

    if (!bestIndex.has_value()) {
        return std::unexpected(Vulkan::Error::NoSuitableDevice);
    }

    return std::move(physicalDevices[*bestIndex]);
}

std::expected<void, Vulkan::Error> Vulkan::CreateQueueAndDevice(
    // Start Queue creation
    const vk::raii::PhysicalDevice& physicalDevice) {
    auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    auto graphicsQueueFamilyProperty =
        std::ranges::find_if(queueFamilyProperties, [](auto const& qfp) {
            return static_cast<bool>(qfp.queueFlags &
                                     vk::QueueFlagBits::eGraphics);
        });
    auto graphicsIndex = static_cast<uint32_t>(std::distance(
        queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
    // SDL_Vulkan_GetPresentationSupport --> Add support via lambda functions

    constexpr float queueFamilyPriority = 1.0f;
    vk::DeviceQueueCreateInfo deviceQueueCI{
        .queueFamilyIndex = graphicsIndex,
        .queueCount = 1,
        .pQueuePriorities = &queueFamilyPriority,
    };

    // Device creation
    const std::vector<const char*> deviceExtensions{
        vk::KHRSwapchainExtensionName,
    };

    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan12Features,
                       vk::PhysicalDeviceVulkan13Features>
        featureChain = {
            {
                .features =
                    {
                        .samplerAnisotropy = vk::True,
                    },
            },
            {
                .descriptorIndexing = vk::True,
                .shaderSampledImageArrayNonUniformIndexing = vk::True,
                .descriptorBindingVariableDescriptorCount = vk::True,
                .runtimeDescriptorArray = vk::True,
                .bufferDeviceAddress = vk::True,
            },
            {
                .synchronization2 = vk::True,
                .dynamicRendering = vk::True,
            },
        };

    const vk::DeviceCreateInfo deviceCI{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCI,
        .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
        .ppEnabledExtensionNames = deviceExtensions.data(),
    };

    auto result = physicalDevice.createDevice(deviceCI, nullptr);
    if (!result.has_value()) {
        return std::unexpected(Vulkan::Error::NoSuitableDevice);
    }

    this->device_ = std::move(result.value());

    // Finish queue creation
    this->queue_ = this->device_.getQueue(graphicsIndex, 0);
    return {};
}
