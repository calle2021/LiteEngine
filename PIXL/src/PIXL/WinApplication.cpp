#include "pixlpch.h"
#include "WinApplication.h"
#include "Logger.h"
#include "Window.h"

namespace PIXL {
	int PIXL::WinApplication::Run(PIXL::PIXLSample* pixlSample, WinArgs args)
	{
		PIXL_CORE_INFO("Welcome to PIXL!");
		PIXL::Window::Run(args);
		return 0;
	}
}