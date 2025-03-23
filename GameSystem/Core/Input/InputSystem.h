#pragma once

class InputSystem {
public:
	virtual ~InputSystem() = default;
	virtual void ProcessInput() = 0;
	virtual bool IsKeyPressed(int key) = 0;
	virtual bool IsMouseButtonPressed(int button) = 0;
	virtual void GetMousePosition() = 0;
	static std::unique_ptr<InputSystem> CreateInputSystem();
};