#include "logger.h"

Logger* Logger::get() {
    if (m_static_inst == nullptr) {
        m_static_inst = new Logger;
    }
    return m_static_inst;
}

Logger::Logger() {
    this->init_loggers();
}

Logger::~Logger() {
    this->stop_logging();
    delete m_static_inst;
    m_static_inst = nullptr;
}

void Logger::stop_logging() {
    m_logger->flush();
    m_stop = true;
}

void Logger::init_loggers() {
    using namespace std::chrono_literals;
    auto stderr_sink{ std::make_shared<spdlog::sinks::stderr_color_sink_mt>() };
    auto stdout_sink{ std::make_shared<spdlog::sinks::stdout_color_sink_mt>() };

    stderr_sink->set_level(spdlog::level::err);
    stdout_sink->set_level(spdlog::level::info);

    m_logger = std::make_unique<spdlog::logger>(spdlog::logger{ { "main_logger" },{ stdout_sink, stderr_sink } });

    spdlog::flush_every(0.25s);
}
