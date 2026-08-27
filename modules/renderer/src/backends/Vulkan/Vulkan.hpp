#pragma once

#include <GE/Backend.hpp>
#include <GE/Platform/SurfaceFactory.hpp>
#include <GE/Platform/Window.hpp>
#include <array>
#include <cstdint>
#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vector>

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_USE_STD_EXPECTED
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_ASSERT_ON_RESULT(...)
#include <vk_mem_alloc_raii.hpp>
#include <vulkan/vulkan_raii.hpp>

#define VKApiVersion vk::ApiVersion13

namespace GE::Render::Backends {
class Vulkan : public GE::Render::IBackend {
public:
    enum Error : uint8_t {
        FailedInstanceCreation,
        NoVulkanDevice,
        NoSuitableDevice,
        NoSurfaceFormat,
        FailedDeviceCreation,
        FailedAllocatorCreation,
        FailedSurfaceCreation,
        FailedToGetSurfaceCaps,
        FailedToGetSurfaceFormats,
        FailedSwapchainCreation,
        FailedToGetSwapchainImages,
        FailedSwapchainImageViewCreation,
        FailedDepthImageCreation,
        FailedDepthImageViewCreation,
        FailedShaderDataBufferCreation,
        FailedSemaphoreCreation,
        FailedFenceCreation,
        FailedCommandPoolCreation,
        FailedCommandBufferCreation,
        FailedShaderModuleCreation,
        FailedPipelineLayoutCreation,
        FailedGraphicsPipelineCreation,
        FailedWaitingDevice,
    };

    struct Extensions {
        std::vector<const char*> data;

        Extensions(char const* const* names, uint32_t count) {
            const std::span<const char* const> view{names, count};
            this->data = std::vector<const char*>(view.begin(), view.end());
        }

        Extensions(std::vector<const char*> data) : data(std::move(data)) {}
    };

    struct ShaderDataBuffer {
        vma::raii::Buffer buffer = nullptr;
        vk::DeviceAddress deviceAddress = {};
    };

    Vulkan() = default;
    ~Vulkan() override;

    std::expected<void, Error> Init(
        std::shared_ptr<GE::Platform::Window>& window,
        std::unique_ptr<GE::Platform::SurfaceFactory>& surfaceFactory,
        const std::string& appName, const std::string& engineName,
        Extensions& extensions);

    void RenderPass() override;

    void Resize() override;

private:
    static constexpr uint32_t maxFramesInFlight = 2;
    uint32_t frameIndex_ = 0;

    std::shared_ptr<GE::Platform::Window> window_;

    vk::raii::Context context_;
    vk::raii::Instance instance_ = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger_ = nullptr;
    vk::raii::SurfaceKHR surface_ = nullptr;
    vk::raii::PhysicalDevice physicalDevice_ = nullptr;
    vk::raii::Device device_ = nullptr;
    vk::raii::Queue queue_ = nullptr;
    vk::Extent2D swapchainExtent_;
    vk::SurfaceFormatKHR swapchainSurfaceFormat_;
    vk::raii::SwapchainKHR swapchain_ = nullptr;
    std::vector<vk::Image> swapchainImages_;
    std::vector<vk::raii::ImageView> swapchainImageViews_;
    vma::raii::Allocator alloc_ = nullptr;
    vma::raii::Image depthImage_ = nullptr;
    vk::raii::ImageView depthImageView_ = nullptr;
    vk::Format depthFormat_ = vk::Format::eUndefined;
    std::array<ShaderDataBuffer, maxFramesInFlight> shaderDataBuffers_;
    vk::raii::CommandPool commandPool_ = nullptr;
    std::array<vk::raii::CommandBuffer, maxFramesInFlight> commandBuffers_ = {
        nullptr,
        nullptr,
    };
    std::array<vk::raii::Fence, maxFramesInFlight> fences_ = {
        nullptr,
        nullptr,
    };
    std::array<vk::raii::Semaphore, maxFramesInFlight>
        imageAcquiredSemaphores_ = {
            nullptr,
            nullptr,
        };
    std::vector<vk::raii::Semaphore> renderCompleteSemaphores_;
    vk::raii::Pipeline graphicsPipeline_ = nullptr;
    vk::raii::PipelineLayout pipelineLayout_ = nullptr;

    std::expected<void, Error> CreateInstance(const std::string& appName,
                                              const std::string& engineName,
                                              Extensions& extensions);

    std::expected<void, Error> SetupDebugMessenger();

    std::expected<vk::raii::PhysicalDevice, Error> SelectPhysicalDevice();

    std::expected<uint32_t, Error> CreateQueueAndDevice(
        const vk::raii::PhysicalDevice& physicalDevice);

    std::expected<void, Error> CreateAllocator(
        const vk::raii::Instance& instance,
        const vk::raii::PhysicalDevice& physicalDevice,
        const vk::raii::Device& device);

    std::expected<void, Error> CreateSurface(
        const std::shared_ptr<GE::Platform::Window>& window,
        std::unique_ptr<GE::Platform::SurfaceFactory>& surfaceFactory,
        const vk::raii::Instance& instance);

    std::expected<void, Error> CreateSwapchain(
        const std::shared_ptr<GE::Platform::Window>& window,
        const vk::raii::SurfaceKHR& surface, const vk::raii::Device& device,
        const vk::raii::PhysicalDevice& physicalDevice,
        std::optional<vk::raii::SwapchainKHR> oldSwapchain = std::nullopt);

    std::expected<void, Error> DepthAttachment(
        const vk::raii::Device& device,
        const vk::raii::PhysicalDevice& physicalDevice,
        const vk::Extent2D& swapchainExtent);

    std::expected<void, Error> CreateShaderDataBuffers(
        const vk::raii::Device& device);

    std::expected<void, Error> CreateRenderCompleteSemaphores(
        const vk::raii::Device& device,
        const vk::SemaphoreCreateInfo& semaphoreCI);

    std::expected<void, Error> CreateSyncronizationObjects(
        const vk::raii::Device& device);

    std::expected<void, Error> CreateCommandPool(const vk::raii::Device& device,
                                                 uint32_t queueFamilyIndex);

    std::expected<void, Error> CreateGraphicsPipeline(
        const vk::raii::Device& device);

    static std::expected<vk::raii::ShaderModule, Error> CreateShaderModule(
        const vk::raii::Device& device);
};
}  // namespace GE::Render::Backends
