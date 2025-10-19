#include "Include/LiteEngine.h"
#include "Core/Logging/Logger.h"
#include "Core/Window/Window.h"
#include "Core/Vulkan/Renderer.h"
#include "Core/Camera/OrbitCamera.h"

namespace LiteEngine
{
int Application::Launch(iLiteBox& app)
{
    Logger::Init();
    Window window(1280, 720);
    LiteEngine::OrbitCamera camera(window.GetAspect());
    LiteVulkan::Renderer vulkan(window, camera);
    vulkan.Init();

    while (!window.Close())
    {
        window.Update();
        vulkan.DrawFrame();
        app.Update();
    }
    vulkan.Shutdown();
    window.Destroy();
    return 0;
}
}