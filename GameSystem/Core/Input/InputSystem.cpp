#include "pch.h"
#include "InputSystem.h"
#include "Core/Input/WinInput.h"

namespace GameSystem {
	std::unique_ptr<InputSystem> InputSystem::Create()
	{
		return std::make_unique<WinInput>();
	}
}