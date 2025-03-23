#pragma once
#include "Core/Input/InputSystem.h"

class WinInput : public InputSystem 
{
public:
	WinInput();
	void ProcessInput() override;
	bool IsKeyPressed(int key) override;
	bool IsMouseButtonPressed(int button)  override;
	void GetMousePosition()  override;
};