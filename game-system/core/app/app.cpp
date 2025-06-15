#include "pch.h"
#include "game-system.h"
#include "core/window/window.h"

#if !(defined(WINDOWS) || defined(LINUX))
#error "unsupported platform"
#endif

namespace GameSystem
{
int Application::Launch(std::unique_ptr<GameApp> game)
{
    Logger::Init();
    Window::Init();

    while (true)
    {
        if (Window::Close()) {
            break;
        }
        Window::Update();
        double dt = Window::GetDeltaTime();
        game->Update();
    }
    Window::Destroy();
    return 0;
}
} // namespace GameSystem