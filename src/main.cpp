#include "core/application.h"
#include "event/eventManager.h"

int main() {
    Logger* logger = &Logger::get();

    if (Application application; application.init()) {
        application.loop();
    } else {
        logger->critical("Application start-up failed");
    }

    return 0;
}
