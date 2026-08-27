#include "Viewport.hpp"

#include <GE/Platform/Window.hpp>
#include <GE/Renderer.hpp>
#include <memory>

#include "QtSurfaceFactory.hpp"
#include "QtWindow.hpp"

using namespace GEditor::Widgets;

Viewport::Viewport(QVulkanInstance* qInstance, GE::Render::Renderer* renderer)
    : qInstance_(qInstance), renderer_(renderer) {
    this->setSurfaceType(QSurface::VulkanSurface);
    this->setVulkanInstance(qInstance);
}

void Viewport::exposeEvent(QExposeEvent* event) {
    QWindow::exposeEvent(event);

    if (this->attached_ || !this->isExposed()) {
        return;
    }
    this->attached_ = true;

    auto window = std::make_unique<IViewport::QtWindow>(this);
    auto surfaceFactory =
        std::make_unique<IViewport::QtSurfaceFactory>(this->qInstance_, this);

    auto res =
        this->renderer_->Init(GE::Render::Renderer::Vulkan, std::move(window),
                              std::move(surfaceFactory));
    if (!res.has_value()) {
        this->attached_ = false;
    }
}

void Viewport::resizeEvent(QResizeEvent* event) { QWindow::resizeEvent(event); }

bool Viewport::event(QEvent* event) { return QWindow::event(event); }
