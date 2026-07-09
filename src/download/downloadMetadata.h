#pragma once

#include <string>
#include <filesystem>

enum class DownloadState {
    ERROR = 0,
    CREATED,
    QUEUED,
    PROBING,
    RUNNING,
    PAUSED,
    BACKOFF,
    VERIFYING,
    COMPLETED,
    FAILED,
    CANCELED
};

struct DownloadSpec {
    //todo: should be something like DownloadType source; where "DownloadType" represents the type of protocol download
    std::string             m_url;
    //std::filesystem::path   m_downloaded_path;
    int                     m_user_priority;
    bool                    m_allow_parrel_download;
};

struct DownloadSnapshot {
    int             m_id;
    DownloadState   m_download_state;
    int             m_bytes_download;
    int             m_bytes_total;
    double          m_download_speed;
    //std::string   m_status_text;
    //todo: maybe keep track of time left?
};

