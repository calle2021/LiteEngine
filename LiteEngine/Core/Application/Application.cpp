#include "Include/LiteEngine.h"
#include "Core/Logging/Logger.h"
#include "Core/Window/GLFWindow.h"

namespace LiteEngine
{
int Application::Launch(iLiteBox& app)
{
    Logger::Init();
    GLFWindow window;
    window.Init();
    LiteVulkan::Context vulkan(window);
    vulkan.Init();

    while (!window.Close())
    {
        window.Update();
        vulkan.Update();
        app.Update();
    }
    vulkan.WaitIdle();
    window.Destroy();
    return 0;
}
}