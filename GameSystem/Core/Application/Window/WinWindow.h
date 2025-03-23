#pragma once
#include "Core/Application/Window/Window.h"

namespace GameSystem {
	class WinWindow : public Window
	{
	public:
		WinWindow(WindowProps props);
		bool Update() override;
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		HWND hwnd;
	};
}