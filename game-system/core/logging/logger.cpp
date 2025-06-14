#include "logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

#include "pch.h"

namespace GameSystem
{
std::shared_ptr<spdlog::logger> Logger::core_logger;
std::shared_ptr<spdlog::logger> Logger::client_logger;

void Logger::init()
{
    core_logger = spdlog::stdout_color_mt("CORE");
    core_logger->set_level(spdlog::level::trace);
    client_logger = spdlog::stdout_color_mt("APP");
    client_logger->set_level(spdlog::level::trace);
}
} // namespace GameSystem
