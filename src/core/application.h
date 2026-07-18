#pragma once

#include "logger.h"
#include "event/eventManager.h"
#include "download/downloadController.h"
#include "window.h"
#include "ui/ui.h"

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
    void handle_sdl_event(const SDL_Event& event);

private:
    Logger*             m_logger                = &Logger::get();
    EventManager*       m_event_manager         = &EventManager::get();
    DownloadController  m_download_controller;
    Window              m_window;
    UI                  m_ui;
    bool                m_is_running            = true;
};
