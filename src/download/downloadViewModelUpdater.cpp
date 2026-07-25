#include "downloadViewModelUpdater.h"

void DownloadViewModelUpdater::process_update(const ProgressUpdate& update) const {
    if ( const auto itr = m_view_model.find(update.m_download_id); itr != m_view_model.end() ) {
        itr->second.m_bytes_total = update.m_bytes_total;
        itr->second.m_bytes_downloaded = update.m_bytes_downloaded;
    }
}

void DownloadViewModelUpdater::process_update(const StateChangedUpdate& update) const {
    //todo: finish StateChangedUpdate callback function
}

void DownloadViewModelUpdater::process_update(const CompletedUpdate& update) const {
    //todo: finish CompletedUpdate callback function
}

void DownloadViewModelUpdater::process_update(const FailedUpdate& update) const {
    //todo: finish FailedUpdate callback function
}
