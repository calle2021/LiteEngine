#include "pixlpch.h"
#include "Application.h"
#include "PIXL/PrecisionClock.h"

namespace PIXL {
	std::unique_ptr<Window> Application::m_window;

	void Application::Init()
	{
	}

	void PIXL::Application::Init(WinArgs winargs)
	{

#ifdef _DEBUG
		ALLOCATE_CONSOLE();
#endif
		Logger::Init();
		PIXL_CORE_INFO("Welcome to PIXL!");
		m_window = std::unique_ptr<Window>(Window::Create(winargs));
		PrecisionClock::Init();
	}

	int PIXL::Application::Run(PIXL::GameApp game) 
	{

		while (true) {
			if (ProcessEvents())
				break;
			double dt = PIXL::PrecisionClock::GetDeltaTime();
			PIXL_CORE_INFO(dt);
		}
		// Shutdown
		return 0;
	}

	bool Application::ProcessEvents()
	{
		MSG msg = {};
		bool quit = false;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				quit = true;
		}
		return quit;
	}

}