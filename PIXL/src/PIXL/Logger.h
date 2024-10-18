#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace PIXL{
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

#define PIXL_CORE_TRACE(...)			::PIXL::Logger::GetCoreLogger()->trace(__VA_ARGS__)
#define PIXL_CORE_INFO(...)				::PIXL::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define PIXL_CORE_WARN(...)				::PIXL::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define PIXL_CORE_ERROR(...)			::PIXL::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define PIXL_CORE_CRITICAL(...)			::PIXL::Logger::GetCoreLogger()->critical(__VA_ARGS__)

#define PIXL_APP_TRACE(...)				::PIXL::Logger::GetClientLogger()->trace(__VA_ARGS__)
#define PIXL_APP_INFO(...)				::PIXL::Logger::GetClientLogger()->info(__VA_ARGS__)
#define PIXL_APP_WARN(...)				::PIXL::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define PIXL_APP_ERROR(...)				::PIXL::Logger::GetClientLogger()->error(__VA_ARGS__)
#define PIXL_APP_CRITICAL(...)			::PIXL::Logger::GetClientLogger()->critical(__VA_ARGS__)