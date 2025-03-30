#pragma once
#include "Core/Input/InputSystem.h"
#include "Core/Application/Window/WinWindow.h"

namespace GameSystem {
	class WinInput : public InputSystem 
	{
	public:
		WinInput();
		void ProcessInput() override;
		bool IsKeyPressed(int key) override;
		bool IsMouseButtonPressed(int button)  override;
		point GetMousePosition() override;
	private:
		std::map<int, bool> m_key_state;
		std::map<int, bool> m_mouse_state;
		POINT m_cursor_pos;

	};
}