#pragma once

#include <spdlog/spdlog.h>

namespace GameSystem
{
class Logger
{
  public:
    static void init();
    static std::shared_ptr<spdlog::logger> &get_core_logger()
    {
        return core_logger;
    }
    static std::shared_ptr<spdlog::logger> &get_client_logger()
    {
        return client_logger;
    }

  private:
    static std::shared_ptr<spdlog::logger> core_logger;
    static std::shared_ptr<spdlog::logger> client_logger;
};
} // namespace GameSystem

#define CORE_TRACE(...) ::GameSystem::Logger::get_core_logger()->trace(__VA_ARGS__)
#define CORE_INFO(...) ::GameSystem::Logger::get_core_logger()->info(__VA_ARGS__)
#define CORE_WARN(...) ::GameSystem::Logger::get_core_logger()->warn(__VA_ARGS__)
#define CORE_ERROR(...) ::GameSystem::Logger::get_core_logger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...) ::GameSystem::Logger::get_core_logger()->critical(__VA_ARGS__)

#define APP_TRACE(...) ::GameSystem::Logger::get_client_logger()->trace(__VA_ARGS__)
#define APP_INFO(...) ::GameSystem::Logger::get_client_logger()->info(__VA_ARGS__)
#define APP_WARN(...) ::GameSystem::Logger::get_client_logger()->warn(__VA_ARGS__)
#define APP_ERROR(...) ::GameSystem::Logger::get_client_logger()->error(__VA_ARGS__)
#define APP_CRITICAL(...) ::GameSystem::Logger::get_client_logger()->critical(__VA_ARGS__)