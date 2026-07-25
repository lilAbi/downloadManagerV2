#pragma once

#include "downloadMetadata.h"
#include "downloadControllerEvents.h"
#include "downloader.h"
#include "core/logger.h"

class DownloadController {
public:
    DownloadController() = default;
    ~DownloadController() = default;

    //delete copy/move assignment/constructors
    DownloadController(const DownloadController&) = delete;
    DownloadController(DownloadController&&) = delete;
    DownloadController& operator=(const DownloadController&) = delete;
    DownloadController& operator=(DownloadController&&) = delete;

    //initialize the subsystems
    bool init();
    //void process events in the queue that been queue'd up
    void process_download_update_queue();
    //check if we need to draw the view model
    std::pair<bool, std::unordered_map<int, DownloadSnapshot>*> should_draw_view_model();

private:
    //submit a download
    int submit(DownloadSpecification download_specification);
    //pause a download
    void pause(int download_id);
    //resume a download from being stopped
    void resume(int download_id);
    //stop a download
    void cancel(int download_id);
    //callback responding to on "DownloadSubmit" event
    void on_download_submit_event(std::shared_ptr<DownloadSubmitEvent> event);
    //callback responding to on "StopDownloaderThread" event
    void on_stop_downloader_thread_event(std::shared_ptr<StopDownloaderThreadEvent> event);

private:
    //UUID for unique download operations
    inline static int                           m_download_id_counter = 0;
    //Logger
    Logger*                                     m_logger = &Logger::get();
    //hold downloader thread
    std::jthread                                m_downloader_thread;
    //view model to represent a download and its state
    std::unordered_map<int, DownloadSnapshot>   m_view_model;
    //queue to manage events related to an active transfer for downloading
    ThreadSafeQueue<DownloadUpdate>             m_downloader_update;
    //functor used to manage the "downloader" aspect
    Downloader                                  m_downloader{m_downloader_update};
};
