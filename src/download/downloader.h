#pragma once

#include "downloadMetadata.h"
#include "core/logger.h"
#include <flat_map>
#include <boost/lockfree/queue.hpp>
#include <curl/multi.h>

//todo: find a better way to do this maybe?
//wakeup downloader thread from sleep
inline std::mutex               g_wake_downloader_thread_mutex;
inline std::condition_variable  g_wake_downloader_thread_cv;
inline std::atomic<bool>        g_wake_downloader_thread_flag{true};


//this should one be passed to a thread
/*
 *  ideally this would be a function object that i can pass to a thread
 *  contains the download metadata
 *  stored in a vector
 *
 *  use an event queue to communicate between DownloadController and thread for new task or pauses/cancels
 *  todo:
 *  1. When attempting to shut down app, the thread is asleep and wont do a clean shutdown
 */
//todo: maybe use std::ref instead of pointers

class Downloader {
public:
    Downloader() = delete;
    Downloader(boost::lockfree::queue<Command>* queue, std::flat_map<int, DownloadSpecification>* transfers);
    ~Downloader();
    //delete copy/move assignment/constructors
    Downloader(const Downloader&) = delete;
    Downloader(Downloader&& obj) noexcept ;
    Downloader& operator=(const Downloader&) = delete;
    Downloader& operator=(Downloader&& obj) = delete;

    //function that the thread obj should run
    void operator()(const std::stop_token& stop_token);

private:
    //
    void wait_for_work();
    //process all commands in queue upon run & dispatch work
    void process_command_queue();
    //handle the "submit" command case
    void process_submit_command(int download_id);
    //open a file to be written to
    void prepare_download_location(DownloadSpecification& download_specification);

    static size_t downloader_write_to_file_cb(char *ptr, size_t size, size_t nmemb, void *userdata);

private:
    std::shared_ptr<spdlog::logger>             m_logger = Logger::get().get_downloader_logger();
    //handle to curls multi interface
    CURLM*                                      m_multi_handle = nullptr;
    //shared event queue to be passed to thread
    boost::lockfree::queue<Command>*            m_command_queue;
    //keep track of all transfers that occurred during the lifetime of application
    std::flat_map<int, DownloadSpecification>*  m_transfers;
};



