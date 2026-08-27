#pragma once

#include <GE/Platform/Surface.hpp>
#include <GE/Platform/SurfaceFactory.hpp>
#include <GE/Platform/Window.hpp>
#include <QWindow>
#include <memory>

namespace GEditor::Widgets::IViewport {
class QtSurfaceFactory : public GE::Platform::SurfaceFactory {
public:
    QtSurfaceFactory(QVulkanInstance* qIntance, QWindow* qWindow);

    std::vector<const char*> RequiredInstanceExtensions() override;

    uint64_t InstanceHandle() override;

    std::expected<std::unique_ptr<GE::Platform::Surface>,
                  GE::Platform::Surface::Error>
    CreateSurface(GE::Platform::Window& window,
                  uint64_t instanceHandle) override;

private:
    QVulkanInstance* instance_;
    QWindow* window_;
};
}  // namespace GEditor::Widgets::IViewport
