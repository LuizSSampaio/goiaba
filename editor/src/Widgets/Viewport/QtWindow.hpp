#pragma once

#include <GE/Platform/Window.hpp>
#include <QWindow>
#include <cstdint>
#include <expected>
#include <string>

namespace GEditor::Widgets::IViewport {
class QtWindow : public GE::Platform::Window {
public:
    explicit QtWindow(QWindow* window);
    ~QtWindow() override = default;

    std::expected<void, Error> InitWindow(const std::string& title,
                                          uint32_t width, uint32_t height,
                                          Flag windowFlags) override;

    std::string title() override;
    void SetTitle(const std::string& title) override;

    uint32_t width() override;
    void SetWidth(uint32_t width) override;

    uint32_t height() override;
    void SetHeight(uint32_t height) override;

    void PollEvents() override;

private:
    QWindow* window_;
};
}  // namespace GEditor::Widgets::IViewport
