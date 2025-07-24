#pragma once
#include <spdlog/spdlog.h>
#include <memory>

namespace GameSystem {
class Logger
{
public:
    static void Init();
    static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return m_CoreLogger; }
private:
    static std::shared_ptr<spdlog::logger> m_CoreLogger;
};
}

#define CORE_LOG_TRACE(...)      SPDLOG_LOGGER_CALL(GameSystem::Logger::GetCoreLogger().get(), spdlog::level::trace, __VA_ARGS__)
#define CORE_LOG_INFO(...)       SPDLOG_LOGGER_CALL(GameSystem::Logger::GetCoreLogger().get(), spdlog::level::info, __VA_ARGS__)
#define CORE_LOG_WARN(...)       SPDLOG_LOGGER_CALL(GameSystem::Logger::GetCoreLogger().get(), spdlog::level::warn, __VA_ARGS__)
#define CORE_LOG_ERROR(...)      SPDLOG_LOGGER_CALL(GameSystem::Logger::GetCoreLogger().get(), spdlog::level::err, __VA_ARGS__)
#define CORE_LOG_CRITICAL(...)   SPDLOG_LOGGER_CALL(GameSystem::Logger::GetCoreLogger().get(), spdlog::level::critical, __VA_ARGS__)
