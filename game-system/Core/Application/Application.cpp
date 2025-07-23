#include "Include/game-system.h"
#include "pch.h"
#include "Core/Logging/Logger.h"

namespace GameSystem 
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
        game->Update();
    }
    m_Window.Destroy();
    return 0;
}
}