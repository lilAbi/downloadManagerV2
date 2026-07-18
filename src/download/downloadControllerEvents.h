#pragma once

#include <utility>

#include "event/event.h"
#include "downloadMetadata.h"

struct DownloadSubmitEvent final : public Event{
    explicit DownloadSubmitEvent(std::string source, std::filesystem::path path) : m_source(std::move(source)), m_downloaded_path(std::move(path)) {}
    std::string             m_source{"no_source_found"};
    std::filesystem::path   m_downloaded_path{"~/Downloads/"};
    ~DownloadSubmitEvent() override = default;
};

struct StopDownloaderThreadEvent final : public Event{
    ~StopDownloaderThreadEvent() override = default;
};