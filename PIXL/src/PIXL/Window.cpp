#include "pixlpch.h"
#include "Window.h"

#ifdef _WIN32
#include "PIXL/Platform/Windows/WinWindow.h"
#endif
namespace PIXL {
	std::unique_ptr<Window> Window::Create(WinArgs winargs, const WindowProps& props)
	{
#ifdef _WIN32
		return std::make_unique<Window>(WinWindow(winargs, props));
#else
		PIXL_CORE_ERROR("Unkown platform.");
		return nullptr;
#endif
	}
}