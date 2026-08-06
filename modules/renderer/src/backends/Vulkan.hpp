#pragma once

#include <GE/Backend.hpp>
#include <cstdint>
#include <expected>
#include <memory>
#include <string>

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
    };

    struct Extensions {
        char const* const* names;
        uint32_t count;
    };

    Vulkan() = default;
    ~Vulkan() = default;

    std::expected<void, Error> Init(std::shared_ptr<SDLWindow>& window,
                                    const std::string& appName,
                                    const std::string& engineName,
                                    const Extensions& extensions);

private:
    vk::raii::Context context_;
    vk::raii::Instance instance_ = nullptr;
    vk::raii::SurfaceKHR surface_ = nullptr;
    vk::raii::Device device_ = nullptr;
    vk::raii::Queue queue_ = nullptr;
    vk::raii::SwapchainKHR swapchain_ = nullptr;
    std::vector<vk::Image> swapchainImages_;
    std::vector<vk::raii::ImageView> swapchainImageViews_;
    vma::raii::Allocator alloc_ = nullptr;
    vma::raii::Image depthImage_ = nullptr;
    vk::raii::ImageView depthImageView_ = nullptr;

    std::expected<void, Error> CreateInstance(const std::string& appName,
                                              const std::string& engineName,
                                              const Extensions& extensions);

    std::expected<vk::raii::PhysicalDevice, Error> SelectPhysicalDevice();

    std::expected<void, Error> CreateQueueAndDevice(
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
};
}  // namespace GE::Render::Backends
