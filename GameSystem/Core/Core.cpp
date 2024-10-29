#include "pch.h"
#include "Core/Core.h"
#include "Core/PrecisionClock.h"

#include "EventSystem/Event.h"
#include "EventSystem/MouseEvent.h"

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
		
		MouseEvent event(1, 2);
		std::cout << event << std::endl;

		while (true) {
			if (!m_window->Update())
				break;
			double dt = PrecisionClock::GetDeltaTime();
		}
		// Shutdown
		return 0;
	}

}