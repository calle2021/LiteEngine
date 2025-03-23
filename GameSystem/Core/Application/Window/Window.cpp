#include "pch.h"
#include "Window.h"
#include "Core/Application/Window/WinWindow.h"

namespace GameSystem {
	std::unique_ptr<Window> Window::Create(WindowProps props)
	{
		return std::unique_ptr<Window>(new WinWindow(props));
	}
}