#include "pixlpch.h"
#include "Application.h"
#include "PIXL/PrecisionClock.h"

namespace PIXL {
	std::unique_ptr<Window> Application::m_window;

	int Application::Run(PIXL::PIXLSample* sample, WinArgs winargs) {
		PIXL_CORE_INFO("Welcome to PIXL!");

		m_window = std::unique_ptr<Window>(Window::Create(winargs));
		PIXL::PrecisionClock::Init();

		MSG msg = {};
		while (msg.message != WM_QUIT) {
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			double dt = PIXL::PrecisionClock::GetDeltaTime();
		}
		return static_cast<int>(msg.wParam);
	}

}