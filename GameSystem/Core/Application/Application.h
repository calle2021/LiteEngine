#pragma once
#include "GameApp.h"
#include "Core/Utils/Utility.h"
#include "Core/Application/Window/Window.h"
#include "Core/Input/InputSystem.h"
#include "Core/Application/Clock/Clock.h"

namespace GameSystem {
	class Application
	{
	public:
		Application(GameApp* game_app);
		~Application();
		int Run();
	private:
		GameApp* m_game;
	    std::unique_ptr<Window> m_window;
		std::unique_ptr<InputSystem> m_input_system;
		std::unique_ptr<Clock> m_precision_clock;
	};
}