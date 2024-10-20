#pragma once
#include "GameApp.h"
#include "Utility.h"
#include "Window.h"

namespace PIXL {
	class Application
	{
	public:
		static int Run(PIXL::GameApp game, WinArgs args);
	private:
		static std::unique_ptr<Window> m_window;
	};
}