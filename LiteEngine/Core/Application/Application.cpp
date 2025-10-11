#include "Include/LiteEngine.h"
#include "Core/Logging/Logger.h"
#include "Core/Window/Window.h"
#include "Core/Vulkan/Context.h"
#include "Core/Camera/OrbitCamera.h"

namespace LiteEngine
{
int Application::Launch(iLiteBox& app)
{
    Logger::Init();
    Window window;
    LiteEngine::OrbitCamera cam(window.GetAspect());
    LiteVulkan::Context vulkan(window, cam);
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