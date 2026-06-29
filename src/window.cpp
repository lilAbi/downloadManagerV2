#include "window.h"

Window::Window() {
    SDL_Window* sdl_window_ptr{
        SDL_CreateWindow(
            "downloadManagerV2",
            1920/2,
            1080/2,
            SDL_WINDOW_RESIZABLE
        )
    };

    SDL_GetWindowSurface(sdl_window_ptr);
    SDL_UpdateWindowSurface(sdl_window_ptr);

    m_sdl_window_ptr.reset(sdl_window_ptr);
}

SDL_Window* Window::get_sdl_window() const {
    return m_sdl_window_ptr.get();
}

SDL_Surface* Window::get_sdl_surface() const {
    return SDL_GetWindowSurface(this->get_sdl_window());
}
