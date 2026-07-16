#include "downloadController.h"

#include "event/eventManager.h"

bool DownloadController::init() {
    EventManager& event_manager = EventManager::get();
    event_manager.subscribe(this, &DownloadController::on_download_submit_event);

    //star downloader thread

    m_logger->info("DownloadController Initialized");
    return true;
}

int DownloadController::submit(DownloadSpecification download_spec) {
    //create new uuid for download
    const int new_download_id = DownloadController::m_download_id_counter++;
    //add it to the transfer list
    if (auto [fst, snd] = m_transfers.try_emplace(new_download_id, std::move(download_spec)); snd) {
        m_logger->info("DownloadID added: {}", new_download_id);
    }
    //submit download added command
    m_command_queue.push({DownloadCommand::SUBMIT, new_download_id});
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
    m_logger->info("Download Source: {}", event->m_source);
    m_logger->info("Downloaded Path: {}", event->m_downloaded_path.generic_string());
    //create a new DownloadSpecification obj from incoming event and pass it to be submitted
    this->submit(
        DownloadSpecification{
            std::move(event->m_source),
            std::move(event->m_downloaded_path),
            nullptr,
            DownloadState::CREATED
        }
    );
}
