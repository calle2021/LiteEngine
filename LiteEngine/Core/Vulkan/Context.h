#pragma once
#include <memory>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_platform.h>
#include "Core/Window/Window.h"
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Renderer.h"
#include "Buffers.h"
#include "Assets.h"
#include "Core/Camera/Camera.h"

using namespace LiteEngine;

namespace LiteVulkan {
class Context
{
public:
    Context(Window& window, Camera& cam);
    void Init();
    void Update();
    void WaitIdle();
private:
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
private:
    vk::raii::Context m_Context;
    vk::raii::Instance m_Instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    vk::raii::SurfaceKHR m_Surface = nullptr;
private:
    Window& m_Window;
    LiteEngine::Camera& m_Camera;
private:
    Device m_Device;
    SwapChain m_SwapChain;
    Pipeline m_Pipeline;
    Renderer m_Renderer;
    Buffers m_Buffers;
    Assets m_Assets;
};
}