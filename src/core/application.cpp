#include "application.h"

#include <imgui_impl_sdl3.h>
#include <SDL3/SDL_opengl.h>
#include <imgui_impl_opengl3.h>
#include <curl/curl.h>

Application::~Application() {
    curl_global_cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    //gets called first before the window wrapper but ultimately doesnt matter
    SDL_Quit();
}

bool Application::init() {
    if(const CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT); result != CURLE_OK) {
        m_logger->critical("ERROR: curl_global_init() init failed");
        return false;
    }
    m_logger->trace("CURL Initialized");

    //initialize SDL application
    SDL_SetAppMetadata("downloadManagerV2", "0.1.0", "com.downloadManagerV2");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        m_logger->critical("Error: SDL_Init(): {}", SDL_GetError());
        return false;
    }
    m_logger->trace("SDL Initialized");

    if (!m_window.init()) {
        m_logger->critical("Error: Window did not initialize");
    }

    if (!m_download_controller.init()) {
        m_logger->critical("Error: DownloadController did not initialize");
    }

    return true;
}

void Application::loop() {
    const ImGuiIO& io = ImGui::GetIO(); (void)io;

    bool show_demo_window = true;
    while (m_is_running) {
        SDL_Event event;

        //process "SDL" events
        while (SDL_PollEvent(&event)) {
            this->handle_sdl_event(event);
        }

        //if window is minimized put app to sleep
        if (SDL_GetWindowFlags(m_window.get_sdl_window()) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        //Draw UI
        //m_ui.draw(view_model);
        m_ui.draw();

        ImGui::Render();
        glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_window.get_sdl_window());
    }

}

void Application::handle_sdl_event(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event); //keep
    //close application
    {   //todo: when window is closed both of these events are processed before app closes
        if (event.type == SDL_EVENT_QUIT) {
            m_logger->info("SDL_EVENT_QUIT called");
            m_is_running = false;
        }
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_window.get_sdl_window())) {
            m_logger->info("SDL_EVENT_WINDOW_CLOSE_REQUESTED called");
            m_event_manager->publish(std::make_shared<StopDownloaderThreadEvent>());
            m_is_running = false;
        }
    }
}