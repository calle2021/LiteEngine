#include "pch.h"
#include "Core/Application/CoreApplication.h"
#include "Core/Application/PrecisionClock.h"

namespace GameSystem {
	std::unique_ptr<Window> CoreApplication::m_window;
	std::unique_ptr<InputSystem> CoreApplication::m_input_system;

	void CoreApplication::Init()
	{
		ALLOCATE_CONSOLE();
		Logger::Init();
		m_window = std::unique_ptr<Window>(Window::Create());
		m_input_system = std::unique_ptr<InputSystem>(InputSystem::CreateInputSystem());
		PrecisionClock::Init();

		CORE_INFO("GameSystem Initialized");
	}

	int CoreApplication::Run(GameSystem::GameApp game)
	{
		Init();

		while (true) {
			if (!m_window->Update())
				break;
			double dt = PrecisionClock::GetDeltaTime();
		}
		// Shutdown
		return 0;
	}
}