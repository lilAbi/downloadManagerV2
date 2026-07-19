#include "downloader.h"

Downloader::Downloader(boost::lockfree::queue<Command>* queue, std::flat_map<int, DownloadSpecification>* transfers)
    : m_command_queue(queue), m_transfers(transfers) {
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
        //sleep(1);
        this->process_command_queue();
        int still_running = -1;

        if (const CURLMcode perform_result = curl_multi_perform(m_multi_handle, &still_running); perform_result != CURLM_OK) {
            m_logger->critical("curl_multi_perform() error");
            continue;
        }

        CURLMsg* curl_msg       = nullptr;
        int messages_in_queue   = 0;
        do {
            curl_msg = curl_multi_info_read(m_multi_handle, &messages_in_queue);
            if(curl_msg && (curl_msg->msg == CURLMSG_DONE)) {
                CURL* easy_handle = curl_msg->easy_handle;
                /* curl_msg->data.result holds the error code for the transfer */
                curl_multi_remove_handle(m_multi_handle, easy_handle);
                curl_easy_cleanup(easy_handle);
                m_logger->info("Download Completed");
            }
        } while (curl_msg);

        if (still_running == 0) {
            m_logger->info("No running transfers...");
            m_logger->info("Putting Downloader thread to sleep...");
            //put the thread to sleep since no more transfer are needed
            g_wake_downloader_thread_flag = false;
            continue;
        } else {
            m_logger->info("There are {} transfers ongoing...", still_running);
        }
    }
    //todo: maybe have the downloader controller own this due to crashing in the deconstructor
    if (const CURLMcode cleanup_result{curl_multi_cleanup(m_multi_handle)}; cleanup_result != CURLM_OK) {
        m_logger->critical("curl_multi_cleanup() cleanup error");
    }
    m_logger->trace("Stopping Downloader thread...");
}

void Downloader::process_command_queue() {
    //wake up thread when flag is enabled and queue
    std::unique_lock<std::mutex> unique_lock(g_wake_downloader_thread_mutex);
    g_wake_downloader_thread_cv.wait(
        unique_lock,
        []{return g_wake_downloader_thread_flag.load();}
    );
    unique_lock.unlock();
    m_logger->info("Downloader Thread Awake");
    //process commands in the command queue
    while (!m_command_queue->empty()) {
        Command command;
        if (auto is_popped = m_command_queue->pop(command)) {
            switch (command.m_download_command) {
                case DownloadCommand::DEFAULT:
                    m_logger->critical("Unknown Download Command");
                    break;
                case DownloadCommand::SUBMIT:
                    m_logger->info("SUBMIT Download Command");
                    this->process_submit_command(command.m_id);
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

void Downloader::process_submit_command(int download_id) {
    //todo: might not be thread safe
    if ( m_transfers->contains(download_id) ) {
        auto& download_spec =  m_transfers->at(download_id);
        //open the file handle
        this->prepare_download_location(download_spec);
        //initialize an easy handle
        download_spec.m_handle = curl_easy_init();
        //set the source of download item
        curl_easy_setopt(download_spec.m_handle, CURLOPT_URL, download_spec.m_source.c_str());
        //enable verbose logging
        curl_easy_setopt(download_spec.m_handle, CURLOPT_VERBOSE, 1L);
        //set the write callback function to write the received data to a file
        curl_easy_setopt(download_spec.m_handle, CURLOPT_WRITEFUNCTION, &Downloader::downloader_write_to_file_cb);
        //pass the File handle to the callback function
        curl_easy_setopt(download_spec.m_handle, CURLOPT_WRITEDATA, &download_spec.m_file);
        //add easy handle to initiate download
        curl_multi_add_handle(m_multi_handle, download_spec.m_handle);
    } else {
        m_logger->critical("Download ID not found");
    }
}

void Downloader::prepare_download_location(DownloadSpecification& download_specification) {
    //check if file already been opened
    if (!download_specification.m_file.is_open()) {
        //open a new file to be written to
        download_specification.m_file = std::move(std::fstream(
            download_specification.m_downloaded_path,
            std::ios_base::out | std::ios_base::app
        ));
    } else {
        m_logger->critical("Submitted file to be download already opened...");
    }
}

size_t Downloader::downloader_write_to_file_cb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    //calculate total bytes to write
    const size_t bytes = size * nmemb;
    if (auto* file_handle = static_cast<std::fstream*>(userdata); file_handle->is_open()) {
        file_handle->write(ptr, bytes);
    } else {
        Logger::get().get_downloader_logger()->critical("FILE NOT OPEN IN downloader_write_to_file_cb");
    }
    return bytes;
}

