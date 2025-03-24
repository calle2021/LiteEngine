#pragma once
#include "GameApp.h"
#include "Core/Utils/Utility.h"
#include "Core/Application/Window/Window.h"
#include "Core/Input/InputSystem.h"
#include "Core/Application/PrecisionClock.h"

namespace GameSystem {
	class CoreApplication
	{
	public:
		CoreApplication(GameApp* game_app);
		~CoreApplication();
		int Run();
	private:
		GameApp* m_game;
	    std::unique_ptr<Window> m_window;
		std::unique_ptr<InputSystem> m_input_system;
		std::unique_ptr<PrecisionClock> m_precision_clock;
	};
}