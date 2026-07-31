#pragma once

#include <cstdint>
#include <expected>
#include <string>

#include "../Backend.hpp"

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan.hpp>

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
    ~Vulkan();

    std::expected<void, Error> Init(const std::string& appName,
                                    const std::string& engineName,
                                    const Extensions& extensions);

private:
    vk::Instance instance_;
    vk::Device device_;
    vk::Queue queue_;

    std::expected<void, Error> CreateInstance(const std::string& appName,
                                              const std::string& engineName,
                                              const Extensions& extensions);

    std::expected<vk::PhysicalDevice, Error> SelectPhysicalDevice();
    static std::expected<vk::PhysicalDevice, Vulkan::Error>
    PickBestPhysicalDevice(const std::vector<vk::PhysicalDevice>& devices);

    std::expected<void, Error> CreateQueueAndDevice(
        vk::PhysicalDevice physicalDevice);
};
}  // namespace GE::Render::Backend
