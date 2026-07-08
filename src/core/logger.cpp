#include "logger.h"

Logger& Logger::get() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    this->init_loggers();
    this->info("Logger Initialized");
}

Logger::~Logger() {
    this->stop_logging();
}

void Logger::stop_logging() {
    m_logger->flush();
    m_stop = true;
}

void Logger::init_loggers() {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("%^[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] %v%$");
    m_logger = std::make_unique<spdlog::logger>( "downloadManagerV2", spdlog::sinks_init_list{console_sink} );
    m_logger->set_level(spdlog::level::trace);
    m_logger->flush_on(spdlog::level::err);
}
