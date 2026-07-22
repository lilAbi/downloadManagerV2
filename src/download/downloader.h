#pragma once

#include "downloadMetadata.h"
#include "container/commandQueue.h"
#include "core/logger.h"
#include <flat_map>
#include <curl/multi.h>

class Downloader {
public:
    Downloader();
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
    void submit_resume_command(int download_id){};
    void submit_pause_command(int download_id){};
    void submit_cancel_command(int download_id){};

private:
    //wakeup libcurl when new work comes in
    template <typename Function>
    void post(Function&& command);

    //execute commands in the queue
    void process_commands();

    //handle the "submit" command case
    void process_submit_command(int download_id, const DownloadSpecification& download_specification);
    void process_resume_command(int download_id){};
    void process_pause_command(int download_id){};
    void process_cancel_command(int download_id){};

    //open a file to be written to
    void prepare_download_location(ActiveTransfer& active_transfer, const DownloadSpecification& download_specification);
    //setup easy handle
    void setup_easy_handle(ActiveTransfer& active_transfer, const DownloadSpecification& download_specification);
    //callback function for writer
    static size_t downloader_write_to_file_cb(char *ptr, size_t size, size_t nmemb, void *userdata);

private:
    std::shared_ptr<spdlog::logger>     m_logger = Logger::get().get_downloader_logger();
    //handle to curls multi interface
    CURLM*                              m_multi_handle = nullptr;
    //Command queue
    CommandQueue                        m_command_queue;
    //keep track of ids to curl handle
    std::unordered_map<CURL*, int>      m_handle_to_download_id;
    //keep track of active transfers and their state
    std::flat_map<int, ActiveTransfer>  m_active_transfers;
};

template<typename Function>
void Downloader::post(Function&& command) {
    m_command_queue.push( std::forward<Function>(command) );
    if (m_multi_handle) curl_multi_wakeup(m_multi_handle);
}



