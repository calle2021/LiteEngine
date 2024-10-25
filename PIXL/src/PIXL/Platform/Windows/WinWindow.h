#pragma once
#include "PIXL/Window.h"

namespace PIXL {
	class WinWindow : public Window
	{
	public:
		WinWindow(const WindowProps& props);
		bool Update();
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		HWND hwnd;
	};
}