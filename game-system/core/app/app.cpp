#include "Include/game-system.h"
#include "core/vulkan/vulkan-core.h"
#include "core/window/window.h"
#include "pch.h"

namespace GameSystem
{
int Application::Launch(std::unique_ptr<GameApp> game)
{
    Logger::Init();
    Window::Init();
    VulkanCore::Init();

    while (!Window::Close())
    {
        Window::Update();
        double dt = Window::GetDeltaTime();
        game->Update();
    }

    VulkanCore::Destroy();
    Window::Destroy();
    return 0;
}
} // namespace GameSystem