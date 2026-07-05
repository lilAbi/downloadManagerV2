#pragma once

#include "logger.h"
#include "window.h"
#include "ui.h"

class Application {
public:
    Application() = default;
    ~Application();

    //initialize the subsystems
    bool init();

    //application loop
    void loop();

    //delete copy/move assignment/constructors
    Application(const Application&) = delete;
    Application(Application&&) = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&) = delete;

private:
    //event loop
    void handle_sdl_event(SDL_Event& event);

private:
    //DownloadController downloadController;
    Logger* m_logger{Logger::get()};
    Window  m_window;
    UI      m_ui;
    bool    m_is_running = true;
};