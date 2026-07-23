#include "downloader.h"

Downloader::Downloader() {
    if(const CURLcode result = curl_global_init(CURL_GLOBAL_DEFAULT); result != CURLE_OK) {
        m_logger->critical("ERROR: curl_global_init() init failed");
    }
    m_logger->trace("LIBCURL Initialized");
    m_multi_handle.reset(curl_multi_init());
    m_logger->trace("Downloader Initialized: {}", fmt::ptr(this));
}

Downloader::~Downloader() {
    m_logger->trace("Deconstructing Downloader Thread: {}", fmt::ptr(this));
}

void Downloader::operator()(std::stop_token stop_token) {
    m_logger->trace("Starting Downloader thread...");
    while (!stop_token.stop_requested()) {
        m_logger->info("Downloader Thread Awake");

        //execute all commands in queue
        this->process_commands();

        int still_running = 0;
        if (const CURLMcode perform_result = curl_multi_perform(m_multi_handle.get(), &still_running); perform_result != CURLM_OK) {
            m_logger->error("curl_multi_perform() error");
            continue;
        }

        this->process_completions();

        if (stop_token.stop_requested()) break;

        //if there are still running transfers then block until work finished
        if (still_running > 0) {
            int ready = 0;
            //Block until a network event OR curl_multi_wakeup() wakes up blocking function
            if (const CURLMcode poll_result = curl_multi_poll(m_multi_handle.get(), nullptr, 0, 10'000, &ready); poll_result != CURLM_OK) {
                m_logger->error("curl_multi_poll failed");
            }
            continue;
        }

        //pause download thread until work or stop request
        m_logger->info("Blocking Downloader Thread");
        m_command_queue.wait(stop_token);
    }
}

void Downloader::submit_download_command(int download_id, DownloadSpecification download_specification) {
    //submit a "command" which is a lamda that calls an internal member function with specified arguments
    this->post(
        [this, download_id, download_spec = std::move(download_specification)]() mutable {
            this->process_submit_command(download_id, download_spec);
        }
    );
}

void Downloader::process_commands() {
    //passing a lamda function instructing how we want to be able to execute "commands" from the command queue
    m_command_queue.drain(
        [this](std::move_only_function<void()>& command) {
            try {
                command();
            } catch (const std::exception& exception) {
                m_logger->critical( "Unhandled exception while processing reactor command: {}", exception.what() );
            } catch (...) {
                m_logger->critical("Unknown exception while processing reactor command");
            }
        }
    );
}

void Downloader::process_completions() {
    const CURLMsg* curl_msg = nullptr;
    int messages_in_queue   = 0;
    do {
        curl_msg = curl_multi_info_read(m_multi_handle.get(), &messages_in_queue);
        if(curl_msg && (curl_msg->msg == CURLMSG_DONE)) {
            m_logger->trace("Download Completed");
            CURL* easy_handle = curl_msg->easy_handle;
            //get associated download id from libcurl handle ptr
            if (const auto itr = m_handle_to_download_id.find(easy_handle); itr != m_handle_to_download_id.end() ) {
                auto& current_transfer = m_active_transfers[itr->second];
                curl_multi_remove_handle(m_multi_handle.get(), current_transfer.m_easy_handle);
                curl_easy_cleanup(current_transfer.m_easy_handle);
                current_transfer.m_easy_handle = nullptr;
                current_transfer.m_state = DownloadState::COMPLETED;
                m_logger->info("Download Marked Completed");
            }
        }
    } while (curl_msg);
}

void Downloader::process_submit_command(const int download_id, const DownloadSpecification& download_specification) {
    if (!m_active_transfers.contains(download_id)) {
        //associate a Download ID with its transfer metadata
        if (auto [itr, is_added] = m_active_transfers.try_emplace(download_id, ActiveTransfer{ .m_download_id = download_id }); is_added) {
            //setup output file to selected download location
            this->prepare_download_location(itr->second, download_specification);
            //initialize the curl handle and setup easy options
            this->setup_easy_handle(itr->second, download_specification);
            //associate a libcurl handle with a Download ID
            m_handle_to_download_id.try_emplace(itr->second.m_easy_handle, download_id);
        } else {
            m_logger->critical("New Active Transfer Insert Failed");
        }
    } else {
        m_logger->critical("Download ID already exist");
    }
}

void Downloader::prepare_download_location(ActiveTransfer& active_transfer, const DownloadSpecification& download_specification) {
    //check if file already been opened
    if (!active_transfer.m_file.is_open()) {
        //open a new file to be written to
        active_transfer.m_file = std::fstream(download_specification.m_downloaded_path, std::ios_base::out | std::ios_base::app);
    } else {
        m_logger->critical("Submitted file to be download already opened...");
    }
}

void Downloader::setup_easy_handle(ActiveTransfer& active_transfer, const DownloadSpecification& download_specification) {
    if (!active_transfer.m_easy_handle) {
        //initialize an easy handle
        active_transfer.m_easy_handle = curl_easy_init();
        //set the source of download item
        curl_easy_setopt(active_transfer.m_easy_handle, CURLOPT_URL, download_specification.m_source.c_str());
        //enable verbose logging
        curl_easy_setopt(active_transfer.m_easy_handle, CURLOPT_VERBOSE, 1L);
        //set the write callback function to write the received data to a file
        curl_easy_setopt(active_transfer.m_easy_handle, CURLOPT_WRITEFUNCTION, &Downloader::downloader_write_to_file_cb);
        //pass the File handle to the callback function
        //todo: ensure when new downloads get added, the handle ptr does not change
        curl_easy_setopt(active_transfer.m_easy_handle, CURLOPT_WRITEDATA, &active_transfer.m_file);
        //add easy handle to initiate download
        curl_multi_add_handle(m_multi_handle.get(), active_transfer.m_easy_handle);
    } else {
        m_logger->critical("libcurl easy handle already initialized");
    }
}

size_t Downloader::downloader_write_to_file_cb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    const size_t bytes = size * nmemb; ////calculate total bytes to write
    if ( auto* file_handle = static_cast<std::fstream*>(userdata); file_handle && file_handle->is_open() ) {
        file_handle->write(ptr, static_cast<std::streamsize>(bytes));
    } else {
        Logger::get().get_downloader_logger()->critical("FILE NOT OPEN IN downloader_write_to_file_cb");
        return CURL_WRITEFUNC_ERROR;
    }
    return bytes;
}

