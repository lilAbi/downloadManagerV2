#pragma once

#include "downloadMetadata.h"
#include "downloadControllerEvents.h"
#include "downloadCommand.h"

#include "core/logger.h"

#include <boost/lockfree/queue.hpp>


class DownloadController {
public:
    DownloadController() = default;
    ~DownloadController() = default;

    //initialize the subsystems
    bool init();

    int submit(DownloadSpec download_spec);
    void pause(int download_id);
    void resume(int download_id);
    void cancel(int download_id);
    //delete?
    DownloadSnapshot get_snapshot(int download_id);

    //delete copy/move assignment/constructors
    DownloadController(const DownloadController&) = delete;
    DownloadController(DownloadController&&) = delete;
    DownloadController& operator=(const DownloadController&) = delete;
    DownloadController& operator=(DownloadController&&) = delete;

private:
    void on_download_submit_event(std::shared_ptr<DownloadSubmitEvent> event);

private:
    //Logger
    Logger*                             m_logger = &Logger::get();

    //shared event queue to be passed to thread
    boost::lockfree::queue< Command >   m_command_queue{8};

    //keep track of all transfers that occurred during the session
    std::flat_map<int, DownloadSpec>    m_transfers;

    //UUID tracker
    inline static int                   m_download_id_counter = 0;
};
