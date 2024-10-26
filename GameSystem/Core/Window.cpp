#include "pch.h"
#include "Window.h"

#ifdef _WIN32
#include "Core/Platform/Windows/WinWindow.h"
#endif
namespace GameSystem {
	std::unique_ptr<Window> Window::Create(const WindowProps& props)
	{
#ifdef _WIN32
		return std::unique_ptr<Window>(new WinWindow(props));
#else
		PIXL_CORE_ERROR("Unkown platform.");
		return nullptr;
#endif
	}
}