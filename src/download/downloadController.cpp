#include "downloadController.h"
#include "downloader.h"
#include "event/eventManager.h"

bool DownloadController::init() {
    EventManager& event_manager = EventManager::get();

    //register callback function to be called
    event_manager.subscribe(this, &DownloadController::on_download_submit_event);
    event_manager.subscribe(this, &DownloadController::on_stop_downloader_thread_event);

    //start downloader thread, move constructed
    m_downloader_thread = std::jthread(&Downloader::operator(), &m_downloader);

    m_logger->trace("DownloadController Initialized");
    return true;
}

int DownloadController::submit(DownloadSpecification download_specification) {
    const int new_download_id = DownloadController::m_download_id_counter++;
    m_downloader.submit_download_command(new_download_id, std::move(download_specification));
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
            .m_source = std::move(event->m_source),
            .m_downloaded_path = std::move(event->m_downloaded_path),
        }
    );
}

void DownloadController::on_stop_downloader_thread_event(std::shared_ptr<StopDownloaderThreadEvent> event) {
    m_logger->info("StopDownloaderThreadEvent has been called");
    m_downloader_thread.request_stop();
    //block until downloader thread joins
    m_downloader_thread.join();
    m_logger->info("Downloader Thread Joined");
}
