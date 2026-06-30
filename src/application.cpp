#include "application.h"

#include <imgui_impl_sdl3.h>
#include <SDL3/SDL_opengl.h>
#include "../bindings/imgui_impl_opengl3.h"

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
    //gets called first before the window wrapper but ultimately doesnt matter
    SDL_Quit();
}

bool Application::init() {
    SDL_SetAppMetadata("downloadManagerV2", "0.1.0", "com.downloadManagerV2");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMEPAD)) {
        m_logger->info("Error: SDL_Init(): {}", SDL_GetError());
        return false;
    }

    m_window.init();

    return true;
}

void Application::loop() {
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    bool show_demo_window = true;
    while (m_is_running) {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            this->handle_sdl_event(event);
        }

        // [If using SDL_MAIN_USE_CALLBACKS: all code below would likely be your SDL_AppIterate() function]
        if (SDL_GetWindowFlags(m_window.get_sdl_window()) & SDL_WINDOW_MINIMIZED) {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::Render();

        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(m_window.get_sdl_window());
    }

}

void Application::handle_sdl_event(SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event); //keep
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        m_logger->info("Mouse moved");
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        m_logger->info("Mouse clicked");
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
        m_logger->info("Keyboard button pressed");
    }
    if (event.type == SDL_EVENT_QUIT)
        m_is_running = false;
    if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_window.get_sdl_window()))
        m_is_running = false;
}
