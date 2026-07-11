#pragma once
#include "core/logger.h"
#include "event/eventManager.h"

//Data to be shared across different functions of the ui manager
struct UIWindowData {
    //enable popup windows
    bool            m_show_add_download_window = false;
    // Other data
    //holds the data for the inputted address string
    std::string     m_address;
};

//Should own the view model
class UI {
public:
    //keeps track of the download state
    //void draw(view_model model);

    void draw();

private:
    void draw_pop_up_window();

    void draw_menu_bar();

    void draw_button_header();

    void draw_filesystem_nav_panel();

    void draw_download_list_panel();


private:
    void draw_add_download_window();

private:
    inline static UIWindowData      m_ui_window_data;
    Logger*                         m_logger = &Logger::get();
    EventManager*                   m_event_manager = &EventManager::get();
    bool                            m_show_demo_window = false;

};
