#pragma once
#include <vulkan/vulkan.h>

#include "Core/Window/GLFWindow.h"
#include "VulkanBase.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"

class VulkanContext
{
  public:
    VulkanContext();
    ~VulkanContext();

  public:
    void Init(Window *window);
    void Destroy();

  public:
    VkInstance m_Instance;
    VkPhysicalDevice m_PhysicalDevice;
    VkDevice m_Device;
    VkQueue m_GraphicsQueue;
    VkSurfaceKHR m_Surface;
    VkQueue m_PresentQueue;
    VkSwapchainKHR m_SwapChain;

  public:
    Window *p_Window;
    VulkanBase c_Base;
    VulkanDevice c_DeviceManager;
    VulkanSwapChain c_SwapChain;
    VulkanPipeline m_GraphicsPipeline;

  private:
    void CreateInstance();
    void CreateSurface();
};
