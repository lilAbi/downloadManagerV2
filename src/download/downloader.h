#pragma once

#include "downloadMetadata.h"
#include "container/threadSafeQueue.h"
#include "core/logger.h"
#include <curl/multi.h>
#include <memory>

/*
 *  currently the "downloader" thread handles other functionality
 *  to really improve performance- isolate the actually "perform_result"
 *  calls to its own thread and move the other stuff to the main thread
 */

class Downloader {
    struct CurlMultiDeleter {
        void operator()(CURLM* handle) const noexcept { if (handle) curl_multi_cleanup(handle); }
    };
    using Command = std::move_only_function<void()>;
public:
    //Downloader();
    explicit Downloader(ThreadSafeQueue<DownloadUpdate>& queue);
    ~Downloader();
    //delete copy/move assignment/constructors
    Downloader(const Downloader&) = delete;
    Downloader(Downloader&& obj) = delete ;
    Downloader& operator=(const Downloader&) = delete;
    Downloader& operator=(Downloader&& obj) = delete;

    //function that the thread obj should run
    void operator()(std::stop_token stop_token);

    //"public interface" to submit "commands" to influence an active transfer
    void submit_download_command(int download_id, DownloadSpecification download_specification);
    void submit_resume_command(int download_id){ /*todo: finish submit resume callback function */ };
    void submit_pause_command(int download_id){ /*todo: finish submit pause callback function */ };
    void submit_cancel_command(int download_id){ /*todo: finish submit cancel callback function */ };

private:
    //wakeup libcurl when new work comes in
    template <typename Function>
    void post(Function&& command);

    //execute commands in the queue
    void process_commands();
    //update transfer that were completed
    void process_completions();

    //handle the "submit" command case
    void process_submit_command(int download_id, const DownloadSpecification& download_specification);
    void process_resume_command(int download_id){ /*todo: finish process resume callback function */ };
    void process_pause_command(int download_id){ /*todo: finish process pause callback function */ };
    void process_cancel_command(int download_id){ /*todo: finish process cancel callback function */ };

    //open a file to be written to
    void prepare_download_location(ActiveTransfer& active_transfer, const DownloadSpecification& download_specification);
    //setup easy handle
    void setup_easy_handle(ActiveTransfer& active_transfer, const DownloadSpecification& download_specification);
    //callback function for writer
    static size_t download_to_file_cb(char *ptr, size_t size, size_t nmemb, void *userdata);
    //callback function for progress
    static size_t update_download_progress_cb(void* userdata, curl_off_t download_total, curl_off_t downloaded_now, curl_off_t upload_total, curl_off_t uploaded_now);

private:
    std::shared_ptr<spdlog::logger>             m_logger = Logger::get().get_downloader_logger();
    //handle to curls multi interface
    std::unique_ptr<CURLM, CurlMultiDeleter>    m_multi_handle = nullptr;
    //Command queue
    ThreadSafeQueue<Command>                    m_command_queue;
    //Progress update queue
    ThreadSafeQueue<DownloadUpdate>&            m_progress_update_queue;
    //!!!here im choosing to use an unordered_map because whenever a new element gets added the actual underlying element address does not change
    //associate a curl handle to its download id
    std::unordered_map<CURL*, int>              m_handle_to_download_id;
    //associate a "download id" to its metadata related to its transfer and state
    std::unordered_map<int, ActiveTransfer>     m_active_transfers;
};

template<typename Function>
void Downloader::post(Function&& command) {
    m_command_queue.push( std::forward<Function>(command) );
    if (m_multi_handle) curl_multi_wakeup(m_multi_handle.get());
}



