#pragma once
#include "downloadMetadata.h"
#include "downloadControllerEvents.h"
#include "core/logger.h"


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
    DownloadSnapshot get_snapshot(int download_id);

    //delete copy/move assignment/constructors
    DownloadController(const DownloadController&) = delete;
    DownloadController(DownloadController&&) = delete;
    DownloadController& operator=(const DownloadController&) = delete;
    DownloadController& operator=(DownloadController&&) = delete;

private:
    void on_download_submit_event(std::shared_ptr<DownloadSubmitEvent> event);

private:
    Logger* m_logger = &Logger::get();
};
