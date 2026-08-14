#pragma once

#include <GE/Backend.hpp>
#include <array>
#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <vector>

#include "src/SDLWindow.hpp"

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
        FailedDepthImageCreation,
        FailedDepthImageViewCreation,
        FailedShaderDataBufferCreation,
        FailedSemaphoreCreation,
        FailedFenceCreation,
        FailedCommandPoolCreation,
        FailedCommandBufferCreation,
        FailedShaderModuleCreation,
    };

    struct Extensions {
        std::vector<const char*> data;

        Extensions(char const* const* names, uint32_t count) {
            const std::span<const char* const> view{names, count};
            this->data = std::vector<const char*>(view.begin(), view.end());
        }
    };

    struct ShaderDataBuffer {
        vma::raii::Buffer buffer = nullptr;
        vk::DeviceAddress deviceAddress = {};
    };

    Vulkan() = default;
    ~Vulkan() override = default;

    std::expected<void, Error> Init(std::shared_ptr<SDLWindow>& window,
                                    const std::string& appName,
                                    const std::string& engineName,
                                    Extensions& extensions);

private:
    static constexpr uint32_t maxFramesInFlight = 2;

    vk::raii::Context context_;
    vk::raii::Instance instance_ = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger_ = nullptr;
    vk::raii::SurfaceKHR surface_ = nullptr;
    vk::raii::Device device_ = nullptr;
    vk::raii::Queue queue_ = nullptr;
    vk::raii::SwapchainKHR swapchain_ = nullptr;
    std::vector<vk::Image> swapchainImages_;
    std::vector<vk::raii::ImageView> swapchainImageViews_;
    vma::raii::Allocator alloc_ = nullptr;
    vma::raii::Image depthImage_ = nullptr;
    vk::raii::ImageView depthImageView_ = nullptr;
    std::array<ShaderDataBuffer, maxFramesInFlight> shaderDataBuffers_;
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
    vk::raii::CommandPool commandPool_ = nullptr;

    std::expected<void, Error> CreateInstance(const std::string& appName,
                                              const std::string& engineName,
                                              Extensions& extensions);

    std::expected<vk::raii::PhysicalDevice, Error> SelectPhysicalDevice();

    std::expected<uint32_t, Error> CreateQueueAndDevice(
        const vk::raii::PhysicalDevice& physicalDevice);

    std::expected<void, Error> CreateAllocator(
        const vk::raii::Instance& instance,
        const vk::raii::PhysicalDevice& physicalDevice,
        const vk::raii::Device& device);

    std::expected<void, Error> CreateSurface(
        const std::shared_ptr<SDLWindow>& window,
        const vk::raii::Instance& instance);

    std::expected<void, Error> CreateSwapchain(
        const std::shared_ptr<Window>& window,
        const vk::raii::SurfaceKHR& surface, const vk::raii::Device& device,
        const vk::raii::PhysicalDevice& physicalDevice);

    std::expected<void, Error> DepthAttachment(
        const std::shared_ptr<Window>& window, const vk::raii::Device& device,
        const vk::raii::PhysicalDevice& physicalDevice);

    std::expected<void, Error> CreateShaderDataBuffers(
        const vk::raii::Device& device);

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
