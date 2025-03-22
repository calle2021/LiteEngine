#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace GameSystem {
	class Logger
	{
	public:
		static void Init();
		static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define CORE_TRACE(...)				::GameSystem::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define CORE_INFO(...)				::GameSystem::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define CORE_WARN(...)				::GameSystem::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define CORE_ERROR(...)				::GameSystem::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define CORE_CRITICAL(...)			::GameSystem::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#define APP_TRACE(...)				::GameSystem::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define APP_INFO(...)				::GameSystem::Logger::GetClientLogger()->info(__VA_ARGS__)
#define APP_WARN(...)				::GameSystem::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define APP_ERROR(...)				::GameSystem::Logger::GetClientLogger()->error(__VA_ARGS__)
#define APP_CRITICAL(...)			::GameSystem::Logger::GetClientLogger()->critical(__VA_ARGS__)