#include "Core/Graphics/VulkanContext.h"
#include "Core/Window/GLFWindow.h"
#include "Include/game-system.h"
#include "pch.h"

namespace GameSystem
{
int Application::Launch(std::unique_ptr<GameApp> game)
{
    m_Window.Init();
    m_Context.Init(&m_Window);

    while (!m_Window.Close())
    {
        m_Window.Update();
        double dt = m_Window.GetDeltaTime();
        game->Update();
    }
    m_Window.Destroy();
    return 0;
}
} // namespace GameSystem