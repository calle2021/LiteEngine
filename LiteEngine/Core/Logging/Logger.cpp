#include "Logger.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace LiteEngine {
std::shared_ptr<spdlog::logger> Logger::m_CoreLogger;

void Logger::Init()
{
    auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    sink->set_pattern("[%T] [%n] [%^%l%$] [%s] [%!] [%#] %v");
    m_CoreLogger = std::make_shared<spdlog::logger>("CORE", sink);

    spdlog::register_logger(m_CoreLogger);
    m_CoreLogger->set_level(spdlog::level::trace);
    m_CoreLogger->flush_on(spdlog::level::trace);
}
}