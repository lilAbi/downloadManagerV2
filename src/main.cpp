#include <iostream>

#include "logger.h"
#include "application.h"
#include "curl/curl.h"

int main() {
    Logger* logger = Logger::get();

    CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT);

    if(result != CURLE_OK) {
        logger->critical("Application start-up failed");
    } else {
        if (Application application; application.init()) {
            application.loop();
        } else {
            logger->critical("Application start-up failed");
        }
    }


    curl_global_cleanup();

    return 0;
}
