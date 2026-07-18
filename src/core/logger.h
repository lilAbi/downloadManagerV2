#pragma once

#include <chrono>
#include <string>
#include <type_traits>
#include <flat_map>

#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <spdlog/sinks/callback_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

class Logger {
public:
    static Logger& get();

    template <typename... TArgs>
    void trace(fmt::format_string<TArgs...> format_str, TArgs&&... args) {
        m_default_logger->trace(format_str, std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    void info(fmt::format_string<TArgs...> format_str, TArgs&&... args) {
        m_default_logger->info(format_str, std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    void debug(fmt::format_string<TArgs...> format_str, TArgs&&... args) {
        m_default_logger->debug(format_str, std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    void critical(fmt::format_string<TArgs...> format_str, TArgs&&... args) {
        m_default_logger->critical(format_str, std::forward<TArgs>(args)...);
    }

    std::shared_ptr<spdlog::logger> get_downloader_logger() {
        return m_download_thread_logger;
    }

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    ~Logger();
    Logger();

    void stop_logging() const;

    void init_loggers();

private:
    std::unique_ptr<spdlog::logger> m_default_logger{ nullptr };
    std::shared_ptr<spdlog::logger> m_download_thread_logger{ nullptr };
};
