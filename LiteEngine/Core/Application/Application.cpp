#include "Include/LiteEngine.h"
#include "Core/Logging/Logger.h"

namespace LiteEngine
{
int Application::Launch(std::unique_ptr<iLiteBox> app)
{
    Logger::Init();
    m_Window.Init();
    m_VulkanContext.Init(&m_Window);

    while (!m_Window.Close())
    {
        m_Window.Update();
        double dt = m_Window.GetDeltaTime();
        m_VulkanContext.Update();
        app->Update();
    }
    m_VulkanContext.WaitIdle();
    m_Window.Destroy();
    return 0;
}
}