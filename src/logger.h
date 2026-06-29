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
    using clock_t = std::chrono::high_resolution_clock;
public:
    static Logger* get();

    template <typename... TArgs>
    void info(fmt::format_string<TArgs...> format_str, TArgs&&... args) {
        m_logger->info(format_str, std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    void debug(fmt::format_string<TArgs...> format_str, TArgs&&... args) {
        m_logger->debug(format_str, std::forward<TArgs>(args)...);
    }

    template <typename... TArgs>
    void critical(fmt::format_string<TArgs...> format_str, TArgs&&... args) {
        m_logger->critical(format_str, std::forward<TArgs>(args)...);
    }

private:
    Logger();
    ~Logger();

    void stop_logging();

    void init_loggers();

    Logger(Logger const&)          = delete;
    void operator=(Logger const&)  = delete;

private:
    std::unique_ptr<spdlog::logger> m_logger{ nullptr };
    std::atomic<bool>               m_stop{ false };
    std::atomic<uint32_t>           m_line_num{ 0 };
    const clock_t::time_point       m_start_time{ clock_t::now() };
    static inline Logger*           m_static_inst{ nullptr };

};
