#include "pch.h"
#include "Core/Application/CoreApplication.h"
#include "Core/Application/PrecisionClock.h"

namespace GameSystem {
	CoreApplication::CoreApplication(GameApp* game_app) :
		m_game(game_app)
	{
		ALLOCATE_CONSOLE();
		Logger::Init();
		PrecisionClock::Init();
		m_window = Window::Create();
		m_input_system = InputSystem::CreateInputSystem();
		CORE_INFO("GameSystem initialized");
	}

	CoreApplication::~CoreApplication()
	{
	}

	int CoreApplication::Run()
	{
		while (true) {
			if (!m_window->Update()) {
				break;
			}
			double dt = PrecisionClock::GetDeltaTime();
			m_input_system->ProcessInput();
			m_game->Update();
		}
		// Shutdown
		return 0;
	}
}