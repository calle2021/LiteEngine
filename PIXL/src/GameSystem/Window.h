#pragma once
#include "GameSystem/Utility.h"

namespace GameSystem {
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
		virtual bool Update() = 0;
		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
	};
}