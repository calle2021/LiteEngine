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
		Logger::Init();
#endif
		PIXL_CORE_INFO("Welcome to PIXL!");
		m_window = std::unique_ptr<Window>(Window::Create(winargs));
		PrecisionClock::Init();
	}

	int PIXL::Application::Run(PIXL::GameApp game) 
	{
		MSG msg = {};
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			double dt = PrecisionClock::GetDeltaTime();
		}
		return static_cast<int>(msg.wParam);
	}

}