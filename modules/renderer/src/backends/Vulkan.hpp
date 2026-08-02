#pragma once

#include <GE/Backend.hpp>
#include <cstdint>
#include <expected>
#include <string>

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_USE_STD_EXPECTED
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vk_mem_alloc_raii.hpp>
#include <vulkan/vulkan_raii.hpp>

#define VKApiVersion vk::ApiVersion13

namespace GE::Render::Backends {
class Vulkan : public GE::Render::Backend {
public:
    enum Error : uint8_t {
        FailedInstanceCreation,
        NoVulkanDevice,
        NoSuitableDevice,
        FailedDeviceCreation,
        FailedAllocatorCreation,
    };

    struct Extensions {
        char const* const* names;
        uint32_t count;
    };

    Vulkan() = default;
    ~Vulkan() = default;

    std::expected<void, Error> Init(const std::string& appName,
                                    const std::string& engineName,
                                    const Extensions& extensions);

private:
    vk::raii::Context context_;

    vk::raii::Instance instance_ = nullptr;
    vk::raii::Device device_ = nullptr;
    vk::raii::Queue queue_ = nullptr;
    vma::raii::Allocator alloc_ = nullptr;

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
};
}  // namespace GE::Render::Backends
