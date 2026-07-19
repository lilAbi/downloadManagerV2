#include "downloadController.h"

#include "downloader.h"
#include "event/eventManager.h"

bool DownloadController::init() {
    EventManager& event_manager = EventManager::get();

    //register callback function to be called
    event_manager.subscribe(this, &DownloadController::on_download_submit_event);
    event_manager.subscribe(this, &DownloadController::on_stop_downloader_thread_event);

    //start downloader thread, copy constructed
    m_downloader_thread = std::jthread(Downloader{&m_command_queue, &m_transfers});

    m_logger->trace("DownloadController Initialized");
    return true;
}

int DownloadController::submit(DownloadSpecification download_spec) {
    //create new uuid for download
    const int new_download_id = DownloadController::m_download_id_counter++;
    //add it to the transfer list
    if (auto [position, isAdded] = m_transfers.try_emplace(new_download_id, std::move(download_spec)); isAdded) {
        m_logger->info("DownloadID added: {}", new_download_id);
        //submit download added command
        m_command_queue.push({DownloadCommand::SUBMIT, new_download_id});
    }
    return new_download_id;
}

void DownloadController::pause(int download_id) {

}

void DownloadController::resume(int download_id) {

}

void DownloadController::cancel(int download_id) {

}

DownloadSnapshot DownloadController::get_snapshot(int download_id) {
    return {};
}

void DownloadController::on_download_submit_event(std::shared_ptr<DownloadSubmitEvent> event) {
    m_logger->info("DownloadSubmitEvent has been called");
    m_logger->info("Incoming Download Source: {}", event->m_source);
    m_logger->info("Incoming Downloaded Path: {}", event->m_downloaded_path.generic_string());
    //create a new DownloadSpecification obj from incoming event and pass it to be submitted
    this->submit(
        DownloadSpecification{
            nullptr,
            std::move(event->m_source),
            std::move(event->m_downloaded_path),
            DownloadState::CREATED,
        }
    );
}

void DownloadController::on_stop_downloader_thread_event(std::shared_ptr<StopDownloaderThreadEvent> event) {
    m_logger->info("StopDownloaderThreadEvent has been called");
    m_downloader_thread.request_stop();
    //block until downloader thread joins
    m_downloader_thread.join();
    m_logger->trace("Downloader Thread Joined");
}
