#include "downloader.h"


Downloader::Downloader(boost::lockfree::queue<Command> &queue, std::flat_map<int, DownloadSpecification> &transfers) : m_command_queue(queue), m_transfers(transfers) {
    m_logger.info("Downloader Thread Initialized");
    m_multi_handle = curl_multi_init();
}

Downloader::~Downloader() {
    curl_multi_cleanup(m_multi_handle);
    m_multi_handle = nullptr;
}

void Downloader::operator()(std::stop_token stop_token) {
    while (!stop_token.stop_requested()) {
        sleep(1);

        this->process_command_queue();

        int still_running = 0;

        if (const CURLMcode perform_result = curl_multi_perform(m_multi_handle, &still_running);
            perform_result != CURLM_OK) {
            // Handle/log error.
        }

        process_completions();

        if (!stop_token.stop_requested()) {
            int descriptors_ready = 0;

            const CURLMcode poll_result =
                curl_multi_poll(
                    m_multi_handle,
                    nullptr,
                    0,
                    10'000,
                    &descriptors_ready
                );

            if (poll_result != CURLM_OK) {
                // Handle/log error.
            }
        }
    }

    cleanup_active_downloads();
}

void Downloader::process_command_queue() {

}

void Downloader::process_completions() {
}

void Downloader::cleanup_active_downloads() {
}

