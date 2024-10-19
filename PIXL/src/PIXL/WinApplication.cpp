#include "pixlpch.h"
#include "WinApplication.h"
#include "Window.h"
#include "PrecisionClock.h"

namespace PIXL {
	int PIXL::WinApplication::Run(PIXL::PIXLSample* pixlSample, WinArgs args)
	{
		PIXL_CORE_INFO("Welcome to PIXL!");

		PIXL::Window::Init(args);
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