#pragma once

#include <string>
#include <filesystem>
#include <curl/curl.h>

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

enum class DownloadCommand {
    ERROR = 0,
    SUBMIT,
    PAUSE,
    RESUME,
    CANCEL
};

struct Command {
    DownloadCommand m_download_command{DownloadCommand::ERROR};
    int             m_id{};
};


struct DownloadSpecification {
    std::string             m_source;
    std::filesystem::path   m_downloaded_path;
    CURL*                   m_handle{nullptr};
    DownloadState           m_download_state{DownloadState::ERROR};
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

