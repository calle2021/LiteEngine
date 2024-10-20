#pragma once
#include "PIXL/Window.h"

namespace PIXL {
	class WinWindow : public Window
	{
	public:
		WinWindow(WinArgs winargs, const WindowProps& props);
		void Init(WinArgs winargs, const WindowProps& props);
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		HWND hwnd;
	};
}