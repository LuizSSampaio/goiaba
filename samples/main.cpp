#include <GE/Platform/Surface.hpp>
#include <GE/Platform/SurfaceFactory.hpp>
#include <GE/Platform/Window.hpp>
#include <GE/Renderer.hpp>
#include "GE/Logger.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.h>

#include <array>
#include <cstdint>
#include <expected>
#include <memory>
#include <string>
#include <vector>

using namespace GE::Platform;

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

namespace {
class SDLWindow : public Window {
public:
    ~SDLWindow() override {
        if (this->window_ != nullptr) {
            SDL_DestroyWindow(this->window_);
        }
    }

    std::expected<void, Error> InitWindow(const std::string& title,
                                          uint32_t width, uint32_t height,
                                          Flag windowFlags) override {
        this->SetTitle(title);
        this->SetWidth(width);
        this->SetHeight(height);

        this->window_ = SDL_CreateWindow(
            this->title_.c_str(), static_cast<int>(width),
            static_cast<int>(height),
            ToSDLFlags(windowFlags) | SDL_WINDOW_VULKAN);
        if (this->window_ == nullptr) {
            return std::unexpected(Error::FailToCreateWindow);
        }
        return {};
    }

    std::string title() override { return this->title_; }
    void SetTitle(const std::string& title) override {
        this->title_ = title;
        if (this->window_ == nullptr) {
            return;
        }

        SDL_SetWindowTitle(this->window_, this->title_.c_str());
    }

    uint32_t width() override { return this->width_; }
    void SetWidth(uint32_t width) override {
        this->width_ = width;
        if (this->window_ == nullptr) {
            return;
        }

        SDL_SetWindowSize(this->window_, static_cast<int>(this->width_),
                          static_cast<int>(this->height_));
    }

    uint32_t height() override { return this->height_; }
    void SetHeight(uint32_t height) override {
        this->height_ = height;
        if (this->window_ == nullptr) {
            return;
        }

        SDL_SetWindowSize(this->window_, static_cast<int>(this->width_),
                          static_cast<int>(this->height_));
    }

    void PollEvents() override {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
        }
    }

    SDL_Window* sdlWindow() const { return this->window_; }

private:
    SDL_Window* window_ = nullptr;
    std::string title_;
    uint32_t width_ = 0;
    uint32_t height_ = 0;
};

class SDLSurface : public Surface {
public:
    explicit SDLSurface(VkSurfaceKHR handle) : handle_(handle) {}

    ~SDLSurface() override = default;

    [[nodiscard]] uint64_t nativeHandle() const override {
        return reinterpret_cast<uint64_t>(this->handle_);
    }

private:
    VkSurfaceKHR handle_;
};

class SDLSurfaceFactory : public SurfaceFactory {
public:
    std::vector<const char*> RequiredInstanceExtensions() override {
        Uint32 count = 0;
        const char* const* exts = SDL_Vulkan_GetInstanceExtensions(&count);
        if (exts == nullptr) {
            return {};
        }
        return std::vector<const char*>(exts, exts + count);
    }

    std::expected<std::unique_ptr<Surface>, Surface::Error> CreateSurface(
        Window& window, uint64_t instanceHandle) override {
        auto& sdlWindow = static_cast<SDLWindow&>(window);
        VkInstance instance = reinterpret_cast<VkInstance>(instanceHandle);
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        if (!SDL_Vulkan_CreateSurface(sdlWindow.sdlWindow(), instance, nullptr,
                                      &surface)) {
            return std::unexpected(Surface::Error::FailedToCreateSurface);
        }

        return std::make_unique<SDLSurface>(surface);
    }
};
}  // namespace

int main() {
    GE::Logger::Init();

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        GE::Logger::Shutdown();
        return 1;
    }

    auto window = std::make_unique<SDLWindow>();
    auto windowRes = window->InitWindow("Goiaba", 800, 600,
                                        Window::Flag::Resizable);
    if (!windowRes.has_value()) {
        SDL_Quit();
        GE::Logger::Shutdown();
        return 1;
    }

    auto surfaceFactory = std::make_unique<SDLSurfaceFactory>();

    GE::Render::Renderer renderer;
    auto res = renderer.Init(GE::Render::Renderer::Vulkan, std::move(window),
                             std::move(surfaceFactory));
    if (!res.has_value()) {
        SDL_Quit();
        GE::Logger::Shutdown();
        return 1;
    }

    while (true) {
        renderer.Run();
    }

    SDL_Quit();
    GE::Logger::Shutdown();
    return 0;
}
