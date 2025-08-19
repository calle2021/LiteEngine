#pragma once
#include <memory>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_platform.h>
#include "Core/Window/GLFWindow.h"
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Renderer.h"
#include "Buffers.h"

namespace LiteVulkan {
class Context
{
  public:
    Context(GLFWindow& window);
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
    GLFWindow& m_Window;
  private:
    Device m_Device;
    SwapChain m_SwapChain;
    Pipeline m_Pipeline;
    Renderer m_Renderer;
    Buffers m_Buffers;
};
}