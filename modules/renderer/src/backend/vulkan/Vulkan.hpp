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

    std::expected<void, Error> CreateInstance(const std::string& appName,
                                              const std::string& engineName,
                                              const Extensions& extensions);

    std::expected<void, Error> SelectDevice();
    static std::expected<vk::PhysicalDevice, Vulkan::Error> PickBestDevice(
        const std::vector<vk::PhysicalDevice>& devices);
};
}  // namespace GE::Render::Backend
