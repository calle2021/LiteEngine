#include "pch.h"
#include "Core/Utils/Utility.h"
#include "Core/Application/Application.h"

namespace GameSystem {
	Application::Application(GameApp* game_app)
		: m_game(game_app)
	{
		ALLOCATE_CONSOLE();
		Logger::Init();
		m_precision_clock = Clock::Create();
		m_window = Window::Create();
		m_input_system = InputSystem::Create();
		CORE_INFO("GameSystem initialized");
	}

	Application::~Application()
	{
	}

	int Application::Run()
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