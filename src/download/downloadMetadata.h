#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <curl/curl.h>

enum class DownloadState {
    DEFAULT = 0,
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

//metadata needed to perform a single download
struct DownloadSpecification {
    std::string             m_source;
    std::filesystem::path   m_downloaded_path;
    //future additions
    //Protocol  m_type;
};

//metadata representing the state of an active single transfer
struct DownloadSnapshot {
    int             m_id;
    DownloadState   m_download_state;
    int             m_bytes_download;
    int             m_bytes_total;
    double          m_download_speed;
};

//metadata needed to manage an active single transfer
struct ActiveTransfer {
    int             m_download_id{-1};
    CURL*           m_handle{nullptr};
    std::fstream    m_file{};
    DownloadState   m_state{DownloadState::DEFAULT};
};
