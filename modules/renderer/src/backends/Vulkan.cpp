#include "Vulkan.hpp"

#include <SDL3/SDL_vulkan.h>

#include <GE/Logger.hpp>
#include <algorithm>
#include <cstdint>
#include <expected>
#include <iterator>
#include <utility>

using namespace GE::Render::Backends;

#ifndef NDEBUG
namespace {
VKAPI_ATTR vk::Bool32 VKAPI_CALL
DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
              vk::DebugUtilsMessageTypeFlagsEXT,
              const vk::DebugUtilsMessengerCallbackDataEXT* data, void*) {
    if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) {
        GE::Logger::Error(data->pMessage, GE::Logger::Engine);
    } else if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) {
        GE::Logger::Warn(data->pMessage, GE::Logger::Engine);
    } else {
        GE::Logger::Trace(data->pMessage, GE::Logger::Engine);
    }
    return vk::False;
}
}  // namespace
#endif

std::expected<void, Vulkan::Error> Vulkan::Init(
    std::shared_ptr<SDLWindow>& window, const std::string& appName,
    const std::string& engineName, Extensions& extensions) {
    auto instanceRes = this->CreateInstance(appName, engineName, extensions);
    if (!instanceRes.has_value()) {
        return std::unexpected(instanceRes.error());
    }

    auto physicalDeviceRes = this->SelectPhysicalDevice();
    if (!physicalDeviceRes.has_value()) {
        return std::unexpected(physicalDeviceRes.error());
    }

    auto queueAndDeviceRes =
        this->CreateQueueAndDevice(physicalDeviceRes.value());
    if (!queueAndDeviceRes.has_value()) {
        return std::unexpected(queueAndDeviceRes.error());
    }

    auto allocRes = this->CreateAllocator(
        this->instance_, physicalDeviceRes.value(), this->device_);
    if (!allocRes.has_value()) {
        return std::unexpected(allocRes.error());
    }

    auto surfaceRes = this->CreateSurface(window, this->instance_);
    if (!surfaceRes.has_value()) {
        return std::unexpected(surfaceRes.error());
    }

    auto swapchainRes = this->CreateSwapchain(
        window, this->surface_, this->device_, physicalDeviceRes.value());
    if (!swapchainRes.has_value()) {
        return std::unexpected(swapchainRes.error());
    }

    auto depthAttachRes =
        this->DepthAttachment(window, this->device_, physicalDeviceRes.value());
    if (!depthAttachRes.has_value()) {
        return std::unexpected(depthAttachRes.error());
    }

    return {};
}

std::expected<void, Vulkan::Error> Vulkan::CreateInstance(
    const std::string& appName, const std::string& engineName,
    Extensions& extensions) {
    vk::ApplicationInfo appInfo{
        .pApplicationName = appName.c_str(),
        .pEngineName = engineName.c_str(),
        .apiVersion = VKApiVersion,
    };

    std::vector<const char*> layers;

#ifndef NDEBUG
    extensions.data.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    vk::InstanceCreateInfo instanceCI{
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(layers.size()),
        .ppEnabledLayerNames = layers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.data.size()),
        .ppEnabledExtensionNames = extensions.data.data(),
    };

    auto result = this->context_.createInstance(instanceCI, nullptr);
    if (!result.has_value()) {
        return std::unexpected(Vulkan::Error::FailedInstanceCreation);
    }

    this->instance_ = std::move(result.value());

#ifndef NDEBUG
    vk::DebugUtilsMessengerCreateInfoEXT messengerCI{
        .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                           vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                       vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                       vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
        .pfnUserCallback = DebugCallback,
        .pUserData = nullptr,
    };
    this->debugMessenger_ =
        this->instance_.createDebugUtilsMessengerEXT(messengerCI)
            .value_or(nullptr);
#endif

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
        return std::unexpected(Vulkan::Error::FailedDeviceCreation);
    }

    this->device_ = std::move(result.value());

    // Finish queue creation
    this->queue_ = this->device_.getQueue(graphicsIndex, 0);
    return {};
}

std::expected<void, Vulkan::Error> Vulkan::CreateAllocator(
    const vk::raii::Instance& instance,
    const vk::raii::PhysicalDevice& physicalDevice,
    const vk::raii::Device& device) {
    vma::AllocatorCreateInfo allocatorCI{
        .flags = {},
        .physicalDevice = physicalDevice,
    };
    auto allocRes = vma::raii::createAllocator(instance, device, allocatorCI);
    if (!allocRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedAllocatorCreation);
    }

    this->alloc_ = std::move(allocRes.value());
    return {};
}

std::expected<void, Vulkan::Error> Vulkan::CreateSurface(
    const std::shared_ptr<SDLWindow>& window,
    const vk::raii::Instance& instance) {
    VkSurfaceKHR rawSurface = nullptr;
    // TODO: Remove SDL function from vulkan backend
    if (!SDL_Vulkan_CreateSurface(window->window(),
                                  static_cast<VkInstance>(*instance), nullptr,
                                  &rawSurface)) {
        return std::unexpected(Vulkan::Error::FailedSurfaceCreation);
    }

    this->surface_ = vk::raii::SurfaceKHR(instance, rawSurface);

    return {};
}

std::expected<void, Vulkan::Error> Vulkan::CreateSwapchain(
    const std::shared_ptr<Window>& window, const vk::raii::SurfaceKHR& surface,
    const vk::raii::Device& device,
    const vk::raii::PhysicalDevice& physicalDevice) {
    auto surfaceCapsRes = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    if (!surfaceCapsRes.has_value()) {
        return std::unexpected(Vulkan::FailedToGetSurfaceCaps);
    }

    auto surfaceFormatsRes = physicalDevice.getSurfaceFormatsKHR(surface);
    if (!surfaceFormatsRes.has_value()) {
        return std::unexpected(Vulkan::FailedToGetSurfaceFormats);
    }
    const auto& formats = surfaceFormatsRes.value();
    if (formats.empty()) {
        return std::unexpected(Vulkan::NoSurfaceFormat);
    }

    vk::SurfaceFormatKHR chosen = formats[0];
    for (const auto& format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            chosen = format;
            break;
        }
    }

    const auto& caps = surfaceCapsRes.value();
    vk::Extent2D swapchainExtent;
    constexpr auto waylandDefaultWidth = 0xFFFFFFFF;
    if (caps.currentExtent.width != waylandDefaultWidth) {
        swapchainExtent = caps.currentExtent;
    } else {
        swapchainExtent = vk::Extent2D{
            .width = std::clamp(window->width(), caps.minImageExtent.width,
                                caps.maxImageExtent.width),
            .height = std::clamp(window->height(), caps.minImageExtent.height,
                                 caps.maxImageExtent.height),
        };
    }

    vk::SwapchainCreateInfoKHR swapchainCI = {
        .surface = surface,
        .minImageCount = surfaceCapsRes.value().minImageCount,
        .imageFormat = chosen.format,
        .imageColorSpace = chosen.colorSpace,
        .imageExtent =
            {
                .width = swapchainExtent.width,
                .height = swapchainExtent.height,
            },
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = vk::PresentModeKHR::eFifo,
    };

    auto swapchainRes = device.createSwapchainKHR(swapchainCI);
    if (!swapchainRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedSwapchainCreation);
    }
    this->swapchain_ = std::move(swapchainRes.value());

    auto swapchainImagesRes = this->swapchain_.getImages();
    if (!swapchainImagesRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedToGetSwapchainImages);
    }
    this->swapchainImages_ = swapchainImagesRes.value();

    return {};
}

std::expected<void, Vulkan::Error> Vulkan::DepthAttachment(
    const std::shared_ptr<Window>& window, const vk::raii::Device& device,
    const vk::raii::PhysicalDevice& physicalDevice) {
    std::vector<vk::Format> depthFormatList{
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint,
    };
    vk::Format depthFormat = vk::Format::eUndefined;

    for (auto format : depthFormatList) {
        auto formatProp = physicalDevice.getFormatProperties2(format);
        if (formatProp.formatProperties.optimalTilingFeatures &
            vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
            depthFormat = format;
            break;
        }
    }

    vk::ImageCreateInfo depthImageCI = {
        .imageType = vk::ImageType::e2D,
        .format = depthFormat,
        .extent =
            {
                .width = window->width(),
                .height = window->height(),
                .depth = 1,
            },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = vk::ImageTiling::eOptimal,
        .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    vma::AllocationCreateInfo allocCI = {
        .flags = vma::AllocationCreateFlagBits::eDedicatedMemory,
        .usage = vma::MemoryUsage::eAuto,
    };

    auto depthImageRes = this->alloc_.createImage(depthImageCI, allocCI);
    if (!depthImageRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedDepthImageCreation);
    }

    vk::ImageViewCreateInfo depthViewCI = {
        .image = depthImageRes.value(),
        .viewType = vk::ImageViewType::e2D,
        .format = depthFormat,
        .subresourceRange =
            {
                .aspectMask = vk::ImageAspectFlagBits::eDepth,
                .levelCount = 1,
                .layerCount = 1,
            },
    };

    auto depthImageViewRes = device.createImageView(depthViewCI);
    if (!depthImageViewRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedDepthImageViewCreation);
    }

    this->depthImage_ = std::move(depthImageRes.value());
    this->depthImageView_ = std::move(depthImageViewRes.value());

    return {};
}
