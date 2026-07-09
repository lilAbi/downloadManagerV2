#pragma once

#include "event/event.h"
#include "downloadMetadata.h"
#include <utility>

struct DownloadSubmitEvent final : public Event{
    explicit DownloadSubmitEvent(DownloadSpec download_spec) : m_download_spec(std::move(download_spec)) {}
    DownloadSpec m_download_spec;
    ~DownloadSubmitEvent() override = default;
};