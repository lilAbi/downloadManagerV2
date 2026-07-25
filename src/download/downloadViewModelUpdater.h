#pragma once

#include "downloadMetadata.h"
#include <unordered_map>

class DownloadViewModelUpdater {
public:
    explicit DownloadViewModelUpdater(std::unordered_map<int, DownloadSnapshot>& view_model) : m_view_model(view_model) {};

    void process_update(const ProgressUpdate& update) const;
    void process_update(const StateChangedUpdate& update) const;
    void process_update(const CompletedUpdate& update) const;
    void process_update(const FailedUpdate& update) const;

private:
    std::unordered_map<int, DownloadSnapshot>&  m_view_model;
};

