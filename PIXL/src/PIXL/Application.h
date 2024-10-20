#pragma once
#include "GameApp.h"
#include "Utility.h"
#include "Window.h"

namespace PIXL {
	class Application
	{
	public:
		static void Init();
		static void Init(WinArgs winargs); // Init for windows
		static int Run(PIXL::GameApp game);
	private:
	    static std::unique_ptr<Window> m_window;
	};
}