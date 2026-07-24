#include "logger.h"

Logger& Logger::get() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    this->init_loggers();
    this->trace("Logger Initialized");
}

Logger::~Logger() {
    this->stop_logging();
}

void Logger::stop_logging() const {
    m_default_logger->flush();
}

void Logger::init_loggers() {
    //configure default sink
    auto default_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    default_console_sink->set_level(spdlog::level::trace);
    default_console_sink->set_pattern("\x1b[38;5;88m[%Y-%m-%d %H:%M:%S.%e] [%n] [thread %t]\x1b[0m  %^[%l] %v%$");

    //configure downloader sink
    auto downloader_console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    downloader_console_sink->set_level(spdlog::level::trace);
    downloader_console_sink->set_pattern("\x1b[38;5;55m[%Y-%m-%d %H:%M:%S.%e] [%n] [thread %t]\x1b[0m  %^[%l] %v%$");

    m_default_logger            = std::make_unique<spdlog::logger>( "Main", spdlog::sinks_init_list{default_console_sink} );
    m_download_thread_logger    = std::make_shared<spdlog::logger>( "Downloader", spdlog::sinks_init_list{downloader_console_sink} );

    m_default_logger->set_level(spdlog::level::trace);
    m_default_logger->flush_on(spdlog::level::err);

    m_download_thread_logger->set_level(spdlog::level::trace);
    m_download_thread_logger->flush_on(spdlog::level::err);
}
