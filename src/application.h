#pragma once

#include "logger.h"
#include "window.h"

class Application {
public:
    Application() = default;
    ~Application();

    bool init();

    void loop();

private:
    void handle_sdl_event(SDL_Event& event);

private:
    Logger* m_logger{Logger::get()};
    Window m_window;
    bool m_is_running = true;
};