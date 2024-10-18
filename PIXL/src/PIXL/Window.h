#pragma once
#include "Utility.h"

namespace PIXL {
	class Window
	{
	public:
		static void Init(WinArgs args);
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	private:
		static HWND hwnd;
	};

}