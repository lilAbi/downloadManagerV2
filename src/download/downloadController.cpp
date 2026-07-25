#include "downloadController.h"
#include "downloadViewModelUpdater.h"
#include "event/eventManager.h"

bool DownloadController::init() {
    EventManager& event_manager = EventManager::get();
    //register callback function to be called
    event_manager.subscribe(this, &DownloadController::on_download_submit_event);
    event_manager.subscribe(this, &DownloadController::on_stop_downloader_thread_event);

    //start downloader thread
    m_downloader_thread = std::jthread(&Downloader::operator(), &m_downloader);

    m_logger->trace("DownloadController Initialized");
    return true;
}

void DownloadController::process_download_update_queue() {
    //passes a lamda func that instructs how to execute the commands in the queue
    m_downloader_update.drain([this](DownloadUpdate update_variant) {
            try {
                //visitor pattern to facilitate how to process incoming update
                std::visit(
                    [view_model_updater = DownloadViewModelUpdater{this->m_view_model}] (auto&& update) {
                        view_model_updater.process_update(update);
                    },
                    update_variant
                );
            } catch (const std::exception& exception) {
                m_logger->critical( "Unhandled exception while processing DownloadUpdate: {}", exception.what() );
            } catch (...) {
                m_logger->critical("Unknown exception while processing DownloadUpdate");
            }
        }
    );
}

std::pair<bool, std::unordered_map<int, DownloadSnapshot>*> DownloadController::should_draw_view_model() {
    return{true, &this->m_view_model};
}

int DownloadController::submit(DownloadSpecification download_specification) {
    const int new_download_id = DownloadController::m_download_id_counter++;
    //create a entry in the view model for new active download
    this->m_view_model[new_download_id] = {
        .m_id = new_download_id,
        .m_download_state = DownloadState::CREATED,
        .m_source = download_specification.m_source,
        .m_filename = download_specification.m_downloaded_path,
    };
    //submit new download to downloader thread
    m_downloader.submit_download_command(new_download_id, std::move(download_specification));
    return new_download_id;
}

void DownloadController::pause(int download_id) {
    //todo: finish submitting a pause command to downloader
}

void DownloadController::resume(int download_id) {
    //todo: finish submitting a resume command to downloader
}

void DownloadController::cancel(int download_id) {
    //todo: finish submitting a cancel command to downloader
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
