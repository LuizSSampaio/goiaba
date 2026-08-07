#pragma once

#include <SDL3/SDL.h>

#include <GE/Window.hpp>
#include <cstdint>
#include <expected>
#include <string>

namespace GE::Render {
class SDLWindow : public Window {
public:
    ~SDLWindow() override;

    std::expected<void, Error> InitWindow(const std::string& title,
                                          uint32_t width, uint32_t height,
                                          Flag windowFlags) override;

    std::string title() override;
    void SetTitle(const std::string& title) override;

    uint32_t width() override;
    void SetWidth(uint32_t width) override;

    uint32_t height() override;
    void SetHeight(uint32_t height) override;

    SDL_Window* window();
private:
    SDL_Window* window_ = nullptr;
    std::string title_;
    uint32_t width_;
    uint32_t height_;
};
}  // namespace GE::Render
