#include "QtWindow.hpp"

#include <string>

#include "GE/Platform/Window.hpp"

using namespace GEditor::Widgets::IViewport;

QtWindow::QtWindow(QWindow* window) : window_(window) {}

std::expected<void, GE::Platform::Window::Error> QtWindow::InitWindow(
    const std::string& title, uint32_t width, uint32_t height,
    Flag windowFlags) {
    this->SetTitle(title);
    this->SetWidth(width);
    this->SetHeight(height);
    return {};
}

std::string QtWindow::title() { return this->window_->title().toStdString(); }
void QtWindow::SetTitle(const std::string& title) {
    this->window_->setTitle(QString::fromStdString(title));
}

uint32_t QtWindow::width() { return this->window_->width(); }
void QtWindow::SetWidth(uint32_t width) {
    this->window_->resize(static_cast<int>(width),
                          static_cast<int>(this->height()));
}

uint32_t QtWindow::height() { return this->window_->height(); }
void QtWindow::SetHeight(uint32_t height) {
    this->window_->resize(static_cast<int>(this->width()),
                          static_cast<int>(height));
}

void QtWindow::PollEvents() {};
