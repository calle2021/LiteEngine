#include "pch.h"
#include "Core/Core.h"
#include "Core/PrecisionClock.h"

namespace GameSystem {
	std::unique_ptr<Window> Core::m_window;

	void Core::Init()
	{
		ALLOCATE_CONSOLE();
		Logger::Init();
		m_window = std::unique_ptr<Window>(Window::Create());
		PrecisionClock::Init();

		CORE_INFO("GameSystem Initialized");
	}

	int Core::Run(GameSystem::GameApp game)
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