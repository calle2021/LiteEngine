#include "pch.h"
#include "Window.h"
#include "Core/Application/Window/WinWindow.h"

std::unique_ptr<Window> Window::Create(WindowProps props)
{
	return std::make_unique<GameSystem::WinWindow>(props);
}