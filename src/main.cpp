#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main() {
    std::cout << "Hello, World!" << std::endl;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* Window{SDL_CreateWindow(
      "Hello Window", 800, 300, 0
    )};

    SDL_GetWindowSurface(Window);
    SDL_UpdateWindowSurface(Window);

    bool IsRunning = true;
    SDL_Event Event;
    while (IsRunning) {
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_EVENT_QUIT) {
                IsRunning = false;
            }
        }
    }

    SDL_DestroyWindow(Window);
    SDL_Quit();

    return 0;
}
