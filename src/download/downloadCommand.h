#pragma once

enum class DownloadCommand {
    ERROR = 0,
    SUBMIT,
    PAUSE,
    RESUME,
    CANCEL
};

struct Command {
    //Command(const DownloadCommand command, const int id) : m_download_command(command), m_id(id) {}
    DownloadCommand m_download_command{DownloadCommand::ERROR};
    int             m_id{};
};





