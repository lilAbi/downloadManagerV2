#include "window.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"

bool Window::init() {
    //configure opengl version
    auto glsl_version = "#version 410";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    //configure window flags
    constexpr SDL_WindowFlags window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

    //create sdl window context
    const float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_Window* sdl_window_ptr = SDL_CreateWindow(
        "downloadManagerV2",static_cast<int>(1280 * main_scale),static_cast<int>(800 * main_scale),window_flags
    );
    if (!sdl_window_ptr) {
        m_logger->critical("Error: SDL_CreateWindow(): {}", SDL_GetError());
        return false;
    }

    //create opengl graphics context
    SDL_GLContext gl_context_ptr = SDL_GL_CreateContext(sdl_window_ptr);
    if (!gl_context_ptr) {
        m_logger->critical("Error: SDL_GL_CreateContext(): {}", SDL_GetError());
        return false;
    }
    SDL_GL_MakeCurrent(sdl_window_ptr, gl_context_ptr);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    //set window position and open it
    SDL_SetWindowPosition(sdl_window_ptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(sdl_window_ptr);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.ScaleAllSizes(main_scale);
    // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOpenGL(sdl_window_ptr, gl_context_ptr);
    ImGui_ImplOpenGL3_Init(glsl_version);

    //cache the context ptrs
    m_gl_context.reset(gl_context_ptr);
    m_sdl_window.reset(sdl_window_ptr);

    m_logger->trace("Window Initialized");
    return true;
}

SDL_Window* Window::get_sdl_window() const {
    return m_sdl_window.get();
}

SDL_GLContext Window::get_sdl_gl_context() const {
    return m_gl_context.get();
}

SDL_Surface* Window::get_sdl_surface() const {
    return SDL_GetWindowSurface(this->get_sdl_window());
}
