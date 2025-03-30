#include "pch.h"
#include "WinInput.h"

namespace GameSystem {

	WinInput::WinInput()
	{
		CORE_INFO("WindowsInput Created");
	}

	void WinInput::ProcessInput()
	{
		for (int i = 0; i < 256; i++) {
			if (GetKeyState(i) & 0x8000) {
				m_key_state[i] = true;
				CORE_INFO(i);
			} else {
				m_key_state[i] = false;
			}
		}
		GetCursorPos(&m_cursor_pos);
		ScreenToClient(WinWindow::hwnd, &m_cursor_pos);
	}

	bool WinInput::IsKeyPressed(int key)
	{
		return m_key_state[key];
	}

	bool WinInput::IsMouseButtonPressed(int button)
	{
		return false;
	}

	InputSystem::point WinInput::GetMousePosition()
	{
		return { m_cursor_pos.x, m_cursor_pos.y };
	}
}