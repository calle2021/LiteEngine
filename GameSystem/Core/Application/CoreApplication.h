#pragma once
#include "GameApp.h"
#include "Core/Utils/Utility.h"
#include "Window.h"

namespace GameSystem {
	class Core
	{
	public:
		static void Init();
		static int Run(GameApp game);
	private:
	    static std::unique_ptr<Window> m_window;
	};
}