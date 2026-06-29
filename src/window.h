#pragma once

#include <memory>
#include <SDL3/SDL.h>

class Window {
private:
    struct SDLWindowDeleter {
        void operator()(SDL_Window* sdl_window_ptr) const {
            if (sdl_window_ptr && SDL_WasInit(SDL_INIT_VIDEO)) {
                SDL_DestroyWindow(sdl_window_ptr);
            }
        }
    };

public:
    Window();
    ~Window() = default;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

public:
    [[nodiscard]] inline SDL_Window* get_sdl_window() const;
    [[nodiscard]] SDL_Surface* get_sdl_surface() const;

private:
    std::unique_ptr<SDL_Window, SDLWindowDeleter> m_sdl_window_ptr;

};
