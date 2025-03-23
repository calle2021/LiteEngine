#include "pch.h"
#include "InputSystem.h"
#include "Core/Input/WinInput.h"

std::unique_ptr<InputSystem> InputSystem::CreateInputSystem()
{
	return std::unique_ptr<InputSystem>(new WinInput());
}