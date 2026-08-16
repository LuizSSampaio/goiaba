#include "Vulkan.hpp"

#include <SDL3/SDL_vulkan.h>

#include <GE/Logger.hpp>
#include <algorithm>
#include <cstdint>
#include <expected>
#include <iterator>
#include <utility>

#include "GE/Shader.hpp"
#include "src/ShaderData.hpp"
#include "vulkan/vulkan.hpp"

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

    auto shaderDataBuffersRes = this->CreateShaderDataBuffers(this->device_);
    if (!shaderDataBuffersRes.has_value()) {
        return std::unexpected(shaderDataBuffersRes.error());
    }

    auto syncObjectRes = this->CreateSyncronizationObjects(this->device_);
    if (!syncObjectRes.has_value()) {
        return std::unexpected(syncObjectRes.error());
    }

    auto commandRes =
        this->CreateCommandPool(this->device_, queueAndDeviceRes.value());
    if (!commandRes.has_value()) {
        return std::unexpected(commandRes.error());
    }

    auto pipelineRes = this->CreateGraphicsPipeline(this->device_);
    if (!pipelineRes.has_value()) {
        return std::unexpected(pipelineRes.error());
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

std::expected<uint32_t, Vulkan::Error> Vulkan::CreateQueueAndDevice(
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

    vk::StructureChain<
        vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features>
        featureChain = {
            {
                .features =
                    {
                        .samplerAnisotropy = vk::True,
                    },
            },
            {
                .shaderDrawParameters = vk::True,
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
    return graphicsIndex;
}

std::expected<void, Vulkan::Error> Vulkan::CreateAllocator(
    const vk::raii::Instance& instance,
    const vk::raii::PhysicalDevice& physicalDevice,
    const vk::raii::Device& device) {
    vma::AllocatorCreateInfo allocatorCI{
        .flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress,
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

    this->swapchainSurfaceFormat_ = formats[0];
    for (const auto& format : formats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb &&
            format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            this->swapchainSurfaceFormat_ = format;
            break;
        }
    }

    const auto& caps = surfaceCapsRes.value();
    constexpr auto waylandDefaultWidth = 0xFFFFFFFF;
    if (caps.currentExtent.width != waylandDefaultWidth) {
        this->swapchainExtent_ = caps.currentExtent;
    } else {
        this->swapchainExtent_ = vk::Extent2D{
            .width = std::clamp(window->width(), caps.minImageExtent.width,
                                caps.maxImageExtent.width),
            .height = std::clamp(window->height(), caps.minImageExtent.height,
                                 caps.maxImageExtent.height),
        };
    }

    vk::SwapchainCreateInfoKHR swapchainCI = {
        .surface = surface,
        .minImageCount = surfaceCapsRes.value().minImageCount,
        .imageFormat = this->swapchainSurfaceFormat_.format,
        .imageColorSpace = this->swapchainSurfaceFormat_.colorSpace,
        .imageExtent =
            {
                .width = this->swapchainExtent_.width,
                .height = this->swapchainExtent_.height,
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

std::expected<void, Vulkan::Error> Vulkan::CreateShaderDataBuffers(
    const vk::raii::Device& device) {
    for (uint32_t i = 0; i < Vulkan::maxFramesInFlight; i++) {
        vk::BufferCreateInfo bufferCI = {
            .size = sizeof(ShaderData),
            .usage = vk::BufferUsageFlagBits::eShaderDeviceAddress,
        };

        vma::AllocationCreateInfo bufferAllocCI = {
            .flags =
                vma::AllocationCreateFlagBits::eHostAccessSequentialWrite |
                vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead |
                vma::AllocationCreateFlagBits::eMapped,
            .usage = vma::MemoryUsage::eAuto,
        };

        auto bufferRes = this->alloc_.createBuffer(bufferCI, bufferAllocCI);
        if (!bufferRes.has_value()) {
            return std::unexpected(
                Vulkan::Error::FailedShaderDataBufferCreation);
        }
        this->shaderDataBuffers_[i].buffer = std::move(bufferRes.value());

        vk::BufferDeviceAddressInfo bufferBdaInfo = {
            .buffer = this->shaderDataBuffers_[i].buffer,
        };
        this->shaderDataBuffers_[i].deviceAddress =
            device.getBufferAddress(bufferBdaInfo);
    }

    return {};
}

std::expected<void, Vulkan::Error> Vulkan::CreateSyncronizationObjects(
    const vk::raii::Device& device) {
    vk::SemaphoreCreateInfo semaphoreCI = {};
    vk::FenceCreateInfo fenceCI = {
        .flags = vk::FenceCreateFlagBits::eSignaled,
    };

    for (uint32_t i = 0; i < Vulkan::maxFramesInFlight; i++) {
        auto semaphoreRes = device.createSemaphore(semaphoreCI);
        if (!semaphoreRes.has_value()) {
            return std::unexpected(Vulkan::FailedSemaphoreCreation);
        }
        this->imageAcquiredSemaphores_[i] = std::move(semaphoreRes.value());

        auto fenceRes = device.createFence(fenceCI);
        if (!fenceRes.has_value()) {
            return std::unexpected(Vulkan::FailedFenceCreation);
        }
        this->fences_[i] = std::move(fenceRes.value());
    }

    this->renderCompleteSemaphores_.clear();
    this->renderCompleteSemaphores_.reserve(this->swapchainImages_.size());
    for (uint32_t i = 0; i < this->swapchainImages_.size(); i++) {
        auto semaphoreRes = device.createSemaphore(semaphoreCI);
        if (!semaphoreRes.has_value()) {
            return std::unexpected(Vulkan::FailedSemaphoreCreation);
        }
        this->renderCompleteSemaphores_.push_back(
            std::move(semaphoreRes.value()));
    }

    return {};
}

std::expected<void, Vulkan::Error> Vulkan::CreateCommandPool(
    const vk::raii::Device& device, uint32_t queueFamilyIndex) {
    vk::CommandPoolCreateInfo commandPoolCI = {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = queueFamilyIndex,
    };

    auto cPoolRes = device.createCommandPool(commandPoolCI);
    if (!cPoolRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedCommandPoolCreation);
    }

    vk::CommandBufferAllocateInfo cbAllocCI = {
        .commandPool = cPoolRes.value(),
        .commandBufferCount = Vulkan::maxFramesInFlight,
    };

    auto cbAllocRes = device.allocateCommandBuffers(cbAllocCI);
    if (!cbAllocRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedCommandBufferCreation);
    }

    for (uint32_t i = 0; i < Vulkan::maxFramesInFlight; i++) {
        this->commandBuffers_[i] = std::move(cbAllocRes.value()[i]);
    }

    return {};
}

std::expected<void, Vulkan::Error> Vulkan::CreateGraphicsPipeline(
    const vk::raii::Device& device) {
    auto shaderModuleRes = Vulkan::CreateShaderModule(device);
    if (!shaderModuleRes.has_value()) {
        return std::unexpected(shaderModuleRes.error());
    }

    vk::PipelineShaderStageCreateInfo vertShaderStageCI = {
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = shaderModuleRes.value(),
        .pName = "vertMain",
    };

    vk::PipelineShaderStageCreateInfo fragShaderStageCI{
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = shaderModuleRes.value(),
        .pName = "fragMain",
    };

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
        vertShaderStageCI,
        fragShaderStageCI,
    };

    std::vector<vk::DynamicState> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCI = {
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data(),
    };

    vk::PipelineVertexInputStateCreateInfo vertexInputCI;
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyCI = {
        .topology = vk::PrimitiveTopology::eTriangleList,
    };

    vk::Viewport viewport = {
        .x = 0.0f,
        .y = 0.0f,
        .width = static_cast<float>(this->swapchainExtent_.width),
        .height = static_cast<float>(this->swapchainExtent_.height),
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
    };

    vk::Rect2D scisssor = {
        .offset =
            vk::Offset2D{
                .x = 0,
                .y = 0,
            },
        .extent = this->swapchainExtent_,
    };

    vk::PipelineViewportStateCreateInfo viewportStateCI = {
        .viewportCount = 1,
        .scissorCount = 1,
    };

    vk::PipelineRasterizationStateCreateInfo rasterizationStateCI = {
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = vk::False,
        .lineWidth = 1.0f,
    };

    vk::PipelineMultisampleStateCreateInfo multisamplingStateCI = {
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False,
    };

    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCI = {
        .depthTestEnable = vk::True,
        .depthWriteEnable = vk::True,
        .depthCompareOp = vk::CompareOp::eLessOrEqual,
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{
        .blendEnable = vk::False,
        .colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
    };

    vk::PipelineColorBlendStateCreateInfo colorBlendingCI = {
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutCI = {
        .setLayoutCount = 0,
        .pushConstantRangeCount = 0,
    };
    auto pipelineLayout = device.createPipelineLayout(pipelineLayoutCI);
    if (!pipelineLayout.has_value()) {
        return std::unexpected(Vulkan::Error::FailedPipelineLayoutCreation);
    }

    vk::StructureChain<vk::GraphicsPipelineCreateInfo,
                       vk::PipelineRenderingCreateInfo>
        pipelineCIChain = {
            {
                .stageCount = 2,
                .pStages = shaderStages.data(),
                .pVertexInputState = &vertexInputCI,
                .pInputAssemblyState = &inputAssemblyCI,
                .pViewportState = &viewportStateCI,
                .pRasterizationState = &rasterizationStateCI,
                .pMultisampleState = &multisamplingStateCI,
                .pColorBlendState = &colorBlendingCI,
                .pDynamicState = &dynamicStateCI,
                .layout = std::move(pipelineLayout.value()),
                .renderPass = nullptr,
            },
            {
                .colorAttachmentCount = 1,
                .pColorAttachmentFormats =
                    &this->swapchainSurfaceFormat_.format,
            },
        };

    auto graphicsPipelineRes = device.createGraphicsPipeline(
        nullptr, pipelineCIChain.get<vk::GraphicsPipelineCreateInfo>());
    if (!graphicsPipelineRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedGraphicsPipelineCreation);
    }
    this->graphicsPipeline_ = std::move(graphicsPipelineRes.value());

    return {};
}

std::expected<vk::raii::ShaderModule, Vulkan::Error> Vulkan::CreateShaderModule(
    const vk::raii::Device& device) {
    auto shader = GE::Render::Shader("./samples/shader.spv");

    vk::ShaderModuleCreateInfo shaderModuleCI = {
        .codeSize = shader.data()->size() * sizeof(char),
        .pCode = reinterpret_cast<const uint32_t*>(shader.data()->data()),
    };

    auto shaderModuleRes = device.createShaderModule(shaderModuleCI);
    if (!shaderModuleRes.has_value()) {
        return std::unexpected(Vulkan::Error::FailedShaderModuleCreation);
    }

    return std::move(shaderModuleRes.value());
}
