#pragma once

#include "logger.h"
#include "window.h"

class Application {
public:
    explicit Application(Window& window) : m_window(window) {}

    ~Application() = default;

    void loop();

private:
    void handle_sdl_event(SDL_Event& event);

private:
    Logger* m_logger{Logger::get()};
    Window& m_window;
    bool m_is_running = true;

};