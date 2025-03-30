#pragma once
#include "Core/Application/Window/Window.h"

namespace GameSystem {
	class InputSystem {
	public:
		struct point {
			int x;
			int y;
		};
		virtual ~InputSystem() = default;
		virtual void ProcessInput() = 0;
		virtual bool IsKeyPressed(int key) = 0;
		virtual bool IsMouseButtonPressed(int button) = 0;
		virtual point GetMousePosition() = 0;
		static std::unique_ptr<InputSystem> Create();
	};
}