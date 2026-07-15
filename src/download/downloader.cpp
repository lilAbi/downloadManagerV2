#include "downloader.h"

#include <curl/multi.h>

void Downloader::operator()(std::stop_token stop_token) const {
    /*
    while (!stop_token.stop_requested()) {
        drain_commands();       // submit, pause, resume, cancel
        schedule_transfers();   // honor limits and priorities

        int running = 0;
        check(curl_multi_perform(m_multi, &running));

        process_completions();  // curl_multi_info_read()

        if (!stop_token.stop_requested()) {
            int ready = 0;
            check(curl_multi_poll(
                m_multi,
                nullptr,
                0,
                10'000,
                &ready
            ));
        }
    }

    cancel_and_cleanup_transfers();
    */
}
