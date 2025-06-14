#pragma once
#include "Core/Application/Window/Window.h"

namespace GameSystem {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(WindowProps props);
		bool Update() override;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		static HWND hwnd;
	};
}