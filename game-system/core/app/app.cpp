#include "core/window/window.h"
#include "game-system.h"
#include "pch.h"

#if !(defined(WINDOWS) || defined(LINUX))
#error "unsupported platform"
#endif

namespace GameSystem
{
int Application::Launch(std::unique_ptr<GameApp> game)
{
    Logger::Init();
    Window::Init();

    while (!Window::Close())
    {
        Window::Update();
        double dt = Window::GetDeltaTime();
        game->Update();
    }
    Window::Destroy();
    return 0;
}
} // namespace GameSystem