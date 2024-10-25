#include "pixlpch.h"
#include "Application.h"
#include "PIXL/PrecisionClock.h"

namespace PIXL {
	std::unique_ptr<Window> Application::m_window;

	void PIXL::Application::Init()
	{
#ifdef _DEBUG
		ALLOCATE_CONSOLE();
#endif
		Logger::Init();
		PIXL_CORE_INFO("Welcome to PIXL!");
		m_window = std::unique_ptr<Window>(Window::Create());
		PrecisionClock::Init();
	}

	int PIXL::Application::Run(PIXL::GameApp game)
	{
		Init();

		while (true) {
			if (!m_window->Update())
				break;
			double dt = PIXL::PrecisionClock::GetDeltaTime();
		}
		// Shutdown
		return 0;
	}

}