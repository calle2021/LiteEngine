#include "Include/game-system.h"
#include "pch.h"
#include "Core/Logging/Logger.h"

namespace LiteEngine
{
int Application::Launch(std::unique_ptr<GameApp> game)
{
    Logger::Init();
    m_Window.Init();
    m_VulkanContext.Init(&m_Window);

    while (!m_Window.Close())
    {
        m_Window.Update();
        double dt = m_Window.GetDeltaTime();
        m_VulkanContext.Update();
        game->Update();
    }
    m_VulkanContext.WaitIdle();
    m_Window.Destroy();
    return 0;
}
}