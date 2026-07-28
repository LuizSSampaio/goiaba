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
    enum Error { FailedInstanceCreation };

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

    std::expected<void, vk::Result> CreateInstance(
        const std::string& appName, const std::string& engineName,
        const Extensions& extensions);
};
}  // namespace GE::Render::Backend
