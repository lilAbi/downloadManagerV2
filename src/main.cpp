#include "logger.h"
#include "application.h"

int main() {
    Logger* logger = Logger::get();

    if (Application application; application.init()) {
        application.loop();
    } else {
        logger->critical("Application start-up failed");
    }

    return 0;
}
