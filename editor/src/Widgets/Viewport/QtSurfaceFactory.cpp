#include "QtSurfaceFactory.hpp"

#include <vulkan/vulkan.h>

#include <GE/Platform/Surface.hpp>
#include <GE/Platform/SurfaceFactory.hpp>
#include <GE/Platform/Window.hpp>
#include <QVulkanInstance>
#include <QWindow>
#include <memory>

using namespace GEditor::Widgets::IViewport;

namespace {
class QtSurface : public GE::Platform::Surface {
public:
    explicit QtSurface(VkSurfaceKHR handle) : handle_(handle) {}

    ~QtSurface() override = default;

    [[nodiscard]] uint64_t nativeHandle() const override {
        return reinterpret_cast<uint64_t>(this->handle_);
    }

private:
    VkSurfaceKHR handle_;
};
}  // namespace

QtSurfaceFactory::QtSurfaceFactory(QVulkanInstance* qIntance, QWindow* qWindow)
    : instance_(qIntance), window_(qWindow) {}

std::vector<const char*> QtSurfaceFactory::RequiredInstanceExtensions() {
    return {};
}

uint64_t QtSurfaceFactory::InstanceHandle() {
    return reinterpret_cast<uint64_t>(
        static_cast<VkInstance>(this->instance_->vkInstance()));
}

std::expected<std::unique_ptr<GE::Platform::Surface>,
              GE::Platform::Surface::Error>
QtSurfaceFactory::CreateSurface(GE::Platform::Window& window,
                                uint64_t instanceHandle) {
    (void)window;
    (void)instanceHandle;

    VkSurfaceKHR surface = this->instance_->surfaceForWindow(this->window_);
    if (surface == VK_NULL_HANDLE) {
        return std::unexpected(
            GE::Platform::Surface::Error::FailedToCreateSurface);
    }

    return std::make_unique<QtSurface>(surface);
}
