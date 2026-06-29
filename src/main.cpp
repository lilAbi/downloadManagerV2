#include "logger.h"
#include <SDL3/SDL_main.h>
#include "window.h"
#include "application.h"

int main() {
    Logger* logger = Logger::get();

    SDL_SetAppMetadata("downloadManagerV2", "0.1.0", "com.downloadManagerV2");

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    Window window;

    Application application{window};

    application.loop();

    SDL_Quit();
    return 0;
}
