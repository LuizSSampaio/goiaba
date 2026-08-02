#pragma once

#include <cstdint>
#include <string>

namespace GE::Render {
class Window {
public:
    enum class Flag : uint8_t {
        Fullscreen,
        Occluded,
        Hidden,
        Borderless,
        Resizable,
        Minimized,
        Maximized,
        MouseGrabbed,
        InputFocus,
        MouseFocus,
        Modal,
        HighPixelDensity,
        MouseCapture,
        MouseRelativeMode,
        AlwaysOnTop,
        Utility,
        Tooltip,
        PopupMenu,
        KeyboardGrabbed,
        Transparent,
        NotFocusable,
    };

    virtual ~Window() = default;

    virtual Window CreateWindow(const std::string& title, uint32_t width,
                                uint32_t height, uint8_t flags) = 0;

    virtual std::string title() = 0;
    virtual void SetTitle(const std::string& title) = 0;

    virtual uint32_t width() = 0;
    virtual void SetWidth(uint32_t width) = 0;

    virtual uint32_t height() = 0;
    virtual void SetHeight(uint32_t height) = 0;
};

inline Window::Flag operator|(Window::Flag lhs, Window::Flag rhs) {
    return static_cast<Window::Flag>(
        static_cast<std::underlying_type_t<Window::Flag>>(lhs) |
        static_cast<std::underlying_type_t<Window::Flag>>(rhs));
}

inline Window::Flag operator&(Window::Flag lhs, Window::Flag rhs) {
    return static_cast<Window::Flag>(
        static_cast<std::underlying_type_t<Window::Flag>>(lhs) &
        static_cast<std::underlying_type_t<Window::Flag>>(rhs));
}
}  // namespace GE::Render
