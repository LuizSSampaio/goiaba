#include "Vulkan.hpp"

#include <algorithm>
#include <cstdint>
#include <expected>
#include <iterator>
#include <map>
#include <utility>

using namespace GE::Render::Backend;

Vulkan::~Vulkan() { this->instance_.destroy(); }

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

    vk::Instance instance;
    vk::Result res = vk::createInstance(&instanceCI, nullptr, &instance);

    if (res != vk::Result::eSuccess) {
        return std::unexpected(Vulkan::Error::FailedInstanceCreation);
    }
    this->instance_ = instance;
    return {};
}

std::expected<vk::PhysicalDevice, Vulkan::Error>
Vulkan::SelectPhysicalDevice() {
    auto physicalDevices = this->instance_.enumeratePhysicalDevices();
    if (physicalDevices.empty()) {
        return std::unexpected(Vulkan::Error::NoVulkanDevice);
    }

    auto bestDevice = Vulkan::PickBestPhysicalDevice(physicalDevices);
    if (!bestDevice.has_value()) {
        return std::unexpected(bestDevice.error());
    }
    return bestDevice.value();
}

std::expected<vk::PhysicalDevice, Vulkan::Error> Vulkan::PickBestPhysicalDevice(
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

std::expected<void, Vulkan::Error> Vulkan::CreateQueueAndDevice(
    // Start Queue creation
    vk::PhysicalDevice physicalDevice) {
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

    this->device_ = physicalDevice.createDevice(deviceCI);

    // Finish queue creation
    this->queue_ = this->device_.getQueue(graphicsIndex, 0);
    return {};
}
