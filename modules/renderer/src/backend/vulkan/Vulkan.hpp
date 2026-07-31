#pragma once

#include <cstdint>
#include <expected>
#include <string>

#include "../Backend.hpp"

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_USE_STD_EXPECTED
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#define VKApiVersion vk::ApiVersion13

namespace GE::Render::Backend {
class Vulkan : public IBackend {
public:
    enum Error {
        FailedInstanceCreation,
        NoVulkanDevice,
        NoSuitableDevice,
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

    std::expected<void, Error> CreateInstance(const std::string& appName,
                                              const std::string& engineName,
                                              const Extensions& extensions);

    std::expected<vk::raii::PhysicalDevice, Error> SelectPhysicalDevice();

    std::expected<void, Error> CreateQueueAndDevice(
        const vk::raii::PhysicalDevice& physicalDevice);
};
}  // namespace GE::Render::Backend
