#pragma once

#include <memory>
#include <SDL3/SDL.h>

#include "logger.h"

class Window {
    struct SDLWindowDeleter {
        void operator()(SDL_Window* sdl_window_ptr) const {
            if (sdl_window_ptr && SDL_WasInit(SDL_INIT_VIDEO)) {
                SDL_DestroyWindow(sdl_window_ptr);
            }
        }
    };

    struct SDLGlContextDeleter {
        void operator()(SDL_GLContext sdl_gl_context) const {
            if (sdl_gl_context) {
                SDL_GL_DestroyContext(sdl_gl_context);
            }
        }
    };

public:
    Window() = default;
    ~Window() = default;

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

public:
    bool init();
    [[nodiscard]] inline SDL_Window* get_sdl_window() const;
    [[nodiscard]] SDL_GLContext get_sdl_gl_context() const;
    [[nodiscard]] SDL_Surface* get_sdl_surface() const;

private:
    Logger* m_logger = &Logger::get();
    std::unique_ptr<SDL_Window, SDLWindowDeleter> m_sdl_window;
    std::unique_ptr<SDL_GLContextState, SDLGlContextDeleter> m_gl_context;

};
