#include "pixlpch.h"
#include "Logger.h"


namespace PIXL {
	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

	void PIXL::Logger::Init()
	{
		s_CoreLogger = spdlog::stdout_color_mt("PIXL_CORE");
		s_CoreLogger->set_level(spdlog::level::trace);
		s_ClientLogger = spdlog::stdout_color_mt("PIXL_APP");
		s_ClientLogger->set_level(spdlog::level::trace);
	}
}
