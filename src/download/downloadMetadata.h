#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <boost/url.hpp>
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

enum class DownloadCommand {
    DEFAULT = 0,
    SUBMIT,
    PAUSE,
    RESUME,
    CANCEL
};

struct Command {
    DownloadCommand m_download_command{DownloadCommand::DEFAULT};
    int             m_id{-1};
};

struct DownloadSpecification {
    CURL*                   m_handle{nullptr};
    std::string             m_source;
    std::filesystem::path   m_downloaded_path;
    DownloadState           m_download_state{DownloadState::DEFAULT};
    std::fstream            m_file;
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

