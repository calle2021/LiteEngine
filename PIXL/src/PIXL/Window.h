#pragma once
#include "PIXL/Utility.h"

namespace PIXL {
	struct WindowProps
	{
		const wchar_t* title;
		unsigned int width;
		unsigned int height;
		WindowProps(const wchar_t* t = L"PIXL Window",
			unsigned int w = 1280,
			unsigned int h = 720)
			: title(t), width(w), height(h)
		{
		}
	};
	class Window {
	public:
		virtual ~Window() = default;
		//virtual void OnUpdate() = 0;
		static std::unique_ptr<Window> Create(WinArgs args, const WindowProps& props = WindowProps());
	};
}