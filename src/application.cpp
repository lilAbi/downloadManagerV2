#include "application.h"

void Application::loop() {
    SDL_Event event;
    while (m_is_running) {
        while (SDL_PollEvent(&event)) {
            this->handle_sdl_event(event);
        }
    }
}

void Application::handle_sdl_event(SDL_Event& event) {
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        m_logger->info("Mouse moved");
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        m_logger->info("Mouse clicked");
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
        m_logger->info("Keyboard button pressed");
    }
    if (event.type == SDL_EVENT_QUIT) {
        // User wants to quit...
        m_is_running = false;
    }
}
