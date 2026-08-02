#pragma once

#include <cstdint>
#include <expected>
#include <string>

namespace GE::Render {
class Window {
public:
    enum Error : uint8_t {
        FailToCreateWindow,
    };

    enum class Flag : uint32_t {
        Fullscreen = 1 << 0,
        Occluded = 1 << 1,
        Hidden = 1 << 2,
        Borderless = 1 << 3,
        Resizable = 1 << 4,
        Minimized = 1 << 5,
        Maximized = 1 << 6,
        MouseGrabbed = 1 << 7,
        InputFocus = 1 << 8,
        MouseFocus = 1 << 9,
        Modal = 1 << 10,
        HighPixelDensity = 1 << 11,
        MouseCapture = 1 << 12,
        MouseRelativeMode = 1 << 13,
        AlwaysOnTop = 1 << 14,
        Utility = 1 << 15,
        Tooltip = 1 << 16,
        PopupMenu = 1 << 17,
        KeyboardGrabbed = 1 << 18,
        Transparent = 1 << 19,
        NotFocusable = 1 << 20,
    };

    virtual ~Window() = default;

    virtual std::expected<void, Error> InitWindow(const std::string& title,
                                                  uint32_t width,
                                                  uint32_t height,
                                                  Flag windowFlags) = 0;

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
