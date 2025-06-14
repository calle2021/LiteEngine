#include "Window.h"

#include "LinuxWindow.h"
#include "WindowsWindow.h"
#include "pch.h"

std::unique_ptr<Window> Window::Create(WindowProps props) {
#ifdef WINDOWS_SUBSYSTEM
    return std::make_unique<GameSystem::WindowsWindow>(props);
#elif LINUX_SUBSYSTEM
    return std::make_unique<GameSystem::LinuxWindow>(props);
#endif
}