#pragma once

#include "downloadMetadata.h"
#include "downloadControllerEvents.h"

#include "core/logger.h"

#include <boost/lockfree/queue.hpp>


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
    //submit a download
    int submit(DownloadSpecification download_spec);
    //pause a download
    void pause(int download_id);
    //resume a download from being stopped
    void resume(int download_id);
    //stop a download
    void cancel(int download_id);
    // get a snapshot of a download state - delete?
    DownloadSnapshot get_snapshot(int download_id);

private:
    //callback function to be invoked from ui manager
    void on_download_submit_event(std::shared_ptr<DownloadSubmitEvent> event);

    void on_stop_downloader_thread_event(std::shared_ptr<StopDownloaderThreadEvent> event);

private:
    //UUID for unique download operations
    inline static int                           m_download_id_counter = 0;
    //Logger
    Logger*                                     m_logger = &Logger::get();
    //Shared Command queue
    boost::lockfree::queue<Command>             m_command_queue{0};
    //keep track of all transfers that occurred during the lifetime of application
    std::flat_map<int, DownloadSpecification>   m_transfers;
    //Downloader Thread
    std::jthread                                m_downloader_thread;
};
