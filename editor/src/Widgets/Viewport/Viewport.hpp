#pragma once

#include <GE/Renderer.hpp>
#include <QWindow>

namespace GEditor::Widgets {
class Viewport : public QWindow {
    Q_OBJECT
public:
    Viewport(QVulkanInstance* qInstance, GE::Render::Renderer* renderer);

protected:
    void exposeEvent(QExposeEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    bool event(QEvent*) override;

private:
    QVulkanInstance* qInstance_;
    GE::Render::Renderer* renderer_;
    bool attached_ = false;
};
}  // namespace GEditor::Widgets
