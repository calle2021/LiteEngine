#include "pch.h"
#include "InputSystem.h"

#ifdef _WIN32
#include "Core/Input/WinInput.h"
#endif

std::unique_ptr<InputSystem> InputSystem::CreateInputSystem()
{
#ifdef _WIN32
	return std::unique_ptr<InputSystem>(new WinInput());
#else
	CORE_ERROR("[InputSystem] Unsupported platform");
	return nullptr;
#endif
}