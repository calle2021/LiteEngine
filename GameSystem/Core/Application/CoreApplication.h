#pragma once
#include "GameApp.h"
#include "Core/Utils/Utility.h"
#include "Core/Application/Window/Window.h"
#include "Core/Input/InputSystem.h"

namespace GameSystem {
	class CoreApplication
	{
	public:
		static void Init();
		static int Run(GameApp game);
	private:
	    static std::unique_ptr<Window> m_window;
		static std::unique_ptr<InputSystem> m_input_system;
	};
}