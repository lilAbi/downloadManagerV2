#pragma once

#include "container/threadSafeQueue.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <variant>
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

//update an active download with its progress
struct ProgressUpdate {
    int m_download_id{-1};
    int m_bytes_downloaded{};
    int m_bytes_total{};
    //double m_bytes_per_second{};
};

struct StateChangedUpdate {
    int             m_download_id{-1};
    DownloadState   m_download_state{};
};

struct CompletedUpdate {
    int m_download_id{-1};
};

struct FailedUpdate {
    int         m_download_id{-1};
    std::string m_error;
};

using DownloadUpdate = std::variant<ProgressUpdate, StateChangedUpdate, CompletedUpdate, FailedUpdate>;

//metadata needed to perform a single download
struct DownloadSpecification {
    std::string             m_source;
    std::filesystem::path   m_downloaded_path;
    //future additions
    //Protocol  m_type;
};

//metadata representing the state of an active single transfer
struct DownloadSnapshot {
    int m_id;
    int m_bytes_total;
    int m_bytes_downloaded{-1};
    DownloadState m_download_state;
    std::string m_source;
    std::string m_filename;
};

//metadata needed to manage an active single transfer
struct ActiveTransfer {
    int             m_download_id{-1};
    DownloadState   m_state{DownloadState::DEFAULT};
    CURL*           m_easy_handle = nullptr;
    std::chrono::steady_clock::time_point last_progress_publish{};
    ThreadSafeQueue<DownloadUpdate>*      m_progress_update_queue;
    std::fstream    m_file{};
};