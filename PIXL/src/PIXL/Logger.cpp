#include "pixlpch.h"
#include "Logger.h"


namespace PIXL {
	std::shared_ptr<spdlog::logger> Logger::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Logger::s_ClientLogger;

	void PIXL::Logger::Init()
	{
		s_CoreLogger = spdlog::stdout_color_mt("PIXL_CORE");
		s_ClientLogger = spdlog::stdout_color_mt("PIXL_APP");

		spdlog::set_level(spdlog::level::trace);
	}
}
