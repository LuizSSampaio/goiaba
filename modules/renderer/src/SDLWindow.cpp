#include "SDLWindow.hpp"

#include <SDL3/SDL.h>

#include <GE/Window.hpp>
#include <array>
#include <expected>

using namespace GE::Render;

namespace {
constexpr SDL_WindowFlags ToSDLFlags(Window::Flag flags) {
    struct FlagPair {
        FlagPair(Window::Flag my, SDL_WindowFlags sdl) : my(my), sdl(sdl) {}
        Window::Flag my;
        SDL_WindowFlags sdl;
    };

    const std::array<FlagPair, 21> map{{
        FlagPair(Window::Flag::Fullscreen, SDL_WINDOW_FULLSCREEN),
        FlagPair(Window::Flag::Occluded, SDL_WINDOW_OCCLUDED),
        FlagPair(Window::Flag::Hidden, SDL_WINDOW_HIDDEN),
        FlagPair(Window::Flag::Borderless, SDL_WINDOW_BORDERLESS),
        FlagPair(Window::Flag::Resizable, SDL_WINDOW_RESIZABLE),
        FlagPair(Window::Flag::Minimized, SDL_WINDOW_MINIMIZED),
        FlagPair(Window::Flag::Maximized, SDL_WINDOW_MAXIMIZED),
        FlagPair(Window::Flag::MouseGrabbed, SDL_WINDOW_MOUSE_GRABBED),
        FlagPair(Window::Flag::InputFocus, SDL_WINDOW_INPUT_FOCUS),
        FlagPair(Window::Flag::MouseFocus, SDL_WINDOW_MOUSE_FOCUS),
        FlagPair(Window::Flag::Modal, SDL_WINDOW_MODAL),
        FlagPair(Window::Flag::HighPixelDensity, SDL_WINDOW_HIGH_PIXEL_DENSITY),
        FlagPair(Window::Flag::MouseCapture, SDL_WINDOW_MOUSE_CAPTURE),
        FlagPair(Window::Flag::MouseRelativeMode,
                 SDL_WINDOW_MOUSE_RELATIVE_MODE),
        FlagPair(Window::Flag::AlwaysOnTop, SDL_WINDOW_ALWAYS_ON_TOP),
        FlagPair(Window::Flag::Utility, SDL_WINDOW_UTILITY),
        FlagPair(Window::Flag::Tooltip, SDL_WINDOW_TOOLTIP),
        FlagPair(Window::Flag::PopupMenu, SDL_WINDOW_POPUP_MENU),
        FlagPair(Window::Flag::KeyboardGrabbed, SDL_WINDOW_KEYBOARD_GRABBED),
        FlagPair(Window::Flag::Transparent, SDL_WINDOW_TRANSPARENT),
        FlagPair(Window::Flag::NotFocusable, SDL_WINDOW_NOT_FOCUSABLE),
    }};

    SDL_WindowFlags result = 0;
    auto bits = static_cast<uint32_t>(flags);
    for (const auto& [my, sdl] : map) {
        if (static_cast<bool>(bits & static_cast<uint32_t>(my))) {
            result |= sdl;
        }
    }
    return result;
}
}  // namespace

SDLWindow::~SDLWindow() {
    if (this->window_ != nullptr) {
        SDL_DestroyWindow(this->window_);
    }
}

std::expected<void, Window::Error> SDLWindow::InitWindow(
    const std::string& title, uint32_t width, uint32_t height,
    Window::Flag windowFlags) {
    this->SetTitle(title);
    this->SetWidth(width);
    this->SetHeight(height);

    // TODO: Make possible to also init the window with opengl and metal
    this->window_ = SDL_CreateWindow(
        this->title_.c_str(), static_cast<int>(width), static_cast<int>(height),
        ToSDLFlags(windowFlags) | SDL_WINDOW_VULKAN);
    if (this->window_ == nullptr) {
        return std::unexpected(Window::Error::FailToCreateWindow);
    }
    return {};
}

std::string SDLWindow::title() { return this->title_; }
void SDLWindow::SetTitle(const std::string& title) {
    this->title_ = title;
    if (this->window_ == nullptr) {
        return;
    }

    SDL_SetWindowTitle(this->window_, this->title_.c_str());
}

uint32_t SDLWindow::width() { return this->width_; }
void SDLWindow::SetWidth(uint32_t width) {
    this->width_ = width;
    if (this->window_ == nullptr) {
        return;
    }

    SDL_SetWindowSize(this->window_, static_cast<int>(this->width_),
                      static_cast<int>(this->height_));
}

uint32_t SDLWindow::height() { return this->height_; }
void SDLWindow::SetHeight(uint32_t height) {
    this->height_ = height;
    if (this->window_ == nullptr) {
        return;
    }

    SDL_SetWindowSize(this->window_, static_cast<int>(this->width_),
                      static_cast<int>(this->height_));
}

SDL_Window* SDLWindow::window() { return this->window_; }
