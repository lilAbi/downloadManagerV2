#include "downloadController.h"

#include "event/eventManager.h"

bool DownloadController::init() {
    EventManager* event_manager = &EventManager::get();

    event_manager->subscribe(this, &DownloadController::on_download_submit_event);

    return true;
}

int DownloadController::submit(DownloadSpec download_spec) {
    m_logger->info("Download source: {}", download_spec.m_url);
    m_logger->info("Download priority: {}", download_spec.m_user_priority);

    return 1;
}

void DownloadController::pause(int download_id) {

}

void DownloadController::resume(int download_id) {

}

void DownloadController::cancel(int download_id) {

}

DownloadSnapshot DownloadController::get_snapshot(int download_id) {

}

void DownloadController::on_download_submit_event(std::shared_ptr<DownloadSubmitEvent> event) {
    m_logger->info("DownloadSubmitEvent has been called");
    this->submit( event->m_download_spec );
}
