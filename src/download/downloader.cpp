#include "downloader.h"

Downloader::Downloader(boost::lockfree::queue<Command>* queue, std::flat_map<int, DownloadSpecification>* transfers) : m_command_queue(queue), m_transfers(transfers) {
    m_logger->trace("Downloader Thread Initialized: {}", fmt::ptr(this));
    m_multi_handle = curl_multi_init();
}

Downloader::~Downloader() {
    m_logger->trace("Deconstructing Downloader Thread: {}", fmt::ptr(this));
    if (m_multi_handle) {
        m_multi_handle = nullptr;
    }
}

Downloader::Downloader(Downloader&& obj) noexcept {
    m_logger->trace("Downloader Thread Move Initialized: {}", fmt::ptr(this));
    this->m_command_queue   = obj.m_command_queue;
    this->m_transfers       = obj.m_transfers;
    this->m_multi_handle    = obj.m_multi_handle;
    obj.m_multi_handle      = nullptr;
}

void Downloader::operator()(const std::stop_token& stop_token) {
    m_logger->trace("Starting Downloader thread...");
    while (!stop_token.stop_requested()) {
        m_logger->info("Downloader Thread Awake");
        this->process_command_queue();
        int still_running = 0;

        /*
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
        */

        sleep(2); //todo: remove
    }
    //cleanup_active_downloads();
    //todo: maybe have the downloader controller own this due to crashing in the deconstructor
    if (const CURLMcode cleanup_result{curl_multi_cleanup(m_multi_handle)}; cleanup_result != CURLM_OK) {
        m_logger->critical("curl_multi_cleanup() cleanup error");
    }
    m_logger->trace("Stopping Downloader thread...");
}

void Downloader::process_command_queue() {
    while (!m_command_queue->empty()) {
        Command command;
        if (auto is_popped = m_command_queue->pop(command)) {
            switch (command.m_download_command) {
                case DownloadCommand::DEFAULT:
                    m_logger->critical("Unknown Download Command");
                    this->process_submit_command(command.m_id);
                    break;
                case DownloadCommand::SUBMIT:
                    m_logger->info("SUBMIT Download Command");
                    break;
                case DownloadCommand::PAUSE:
                    m_logger->info("PAUSE Download Command");
                    break;
                case DownloadCommand::RESUME:
                    m_logger->info("RESUME Download Command");
                    break;
                case DownloadCommand::CANCEL:
                    m_logger->info("CANCEL Download Command");
                    break;
                default:
                    m_logger->critical("Unknown Download Command");
                    break;
            }
        }
    }
    m_logger->trace("Finished Processing Command Queue");
}

void Downloader::process_completions() {

}

void Downloader::cleanup_active_downloads() {

}

void Downloader::process_submit_command(int download_id) {
    //todo: might not be thread safe
    if ( m_transfers->contains(download_id) ) {
        auto& download_spec =  m_transfers->at(download_id);
        //initialize the handle
        download_spec.m_handle = curl_easy_init();
        curl_easy_setopt(download_spec.m_handle, CURLOPT_URL, download_spec.m_source.c_str());
        curl_multi_add_handle(m_multi_handle, download_spec.m_handle);
    } else {
        m_logger->critical("Download ID not found");
    }

}

