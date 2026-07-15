#include "downloadController.h"

#include "event/eventManager.h"

bool DownloadController::init() {
    EventManager* event_manager = &EventManager::get();

    event_manager->subscribe(this, &DownloadController::on_download_submit_event);

    return true;
}

int DownloadController::submit(DownloadSpec download_spec) {
    //debug test
    m_logger->info("Download source: {}", download_spec.m_url);
    m_logger->info("Download priority: {}", download_spec.m_user_priority);
    //create new uuid for download
    const int new_download_id = DownloadController::m_download_id_counter++;
    //add it to the transfer list
    m_transfers.try_emplace(new_download_id, std::move(download_spec));
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
    this->submit( event->m_download_spec );
}
