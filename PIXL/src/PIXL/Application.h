#pragma once
#include "PIXLSample.h"
#include "Utility.h"
#include "Window.h"

namespace PIXL {
	class Application
	{
	public:
		static int Run(PIXL::PIXLSample* sample, WinArgs args);
	private:
		static std::unique_ptr<Window> m_window;
	};
}