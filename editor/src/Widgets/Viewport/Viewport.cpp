#include "Viewport.hpp"

using namespace GEditor::Widgets;

Viewport::Viewport(QVulkanInstance* qInstance, GE::Render::Renderer* renderer)
    : qInstance_(qInstance), renderer_(renderer) {}

void Viewport::exposeEvent(QExposeEvent* event) {}
void Viewport::resizeEvent(QResizeEvent* event) {}
bool Viewport::event(QEvent* event) {  // TODO
    return true;
}
