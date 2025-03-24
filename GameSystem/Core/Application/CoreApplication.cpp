#include "pch.h"
#include "Core/Application/CoreApplication.h"

namespace GameSystem {
	CoreApplication::CoreApplication(GameApp* game_app) 
		: m_game(game_app)
		, m_precision_clock(std::make_unique<PrecisionClock>())
	{
		ALLOCATE_CONSOLE();
		Logger::Init();
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
			double dt = m_precision_clock->GetDeltaTime();
			m_input_system->ProcessInput();
			m_game->Update();
		}
		// Shutdown
		return 0;
	}
}