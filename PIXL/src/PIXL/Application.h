#pragma once
#include "GameApp.h"
#include "Utility.h"
#include "Window.h"

namespace PIXL {
	class Application
	{
	public:
		static void Init();
		static int Run(GameApp game);
	private:
	    static std::unique_ptr<Window> m_window;
	};
}