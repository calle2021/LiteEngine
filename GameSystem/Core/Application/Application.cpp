#include "pch.h"
#include "Application.h"
#include "Core/Application/Window/Window.h"
#include "Core/Input/InputSystem.h"
#include "Core/Application/Clock/Clock.h"

namespace GameSystem {
	int Application::Launch(std::unique_ptr<GameApp> game)
	{
		Logger::Init();

		std::unique_ptr<Window> window = Window::Create();
		std::unique_ptr<InputSystem> input_system = InputSystem::Create();
   		std::unique_ptr<Clock> clock = Clock::Create();

		while (true) {
			if (!window->Update()) {
				break;
			}
			double dt = clock->GetDeltaTime();
			input_system->ProcessInput();
			game->Update();			
		}
		return 0;
	}
}