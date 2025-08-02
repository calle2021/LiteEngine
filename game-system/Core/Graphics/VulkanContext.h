#pragma once
#include <memory>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_platform.h>
#include "Core/Window/GLFWindow.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanRenderer.h"

class VulkanDevice;

class VulkanContext
{
  public:
    VulkanContext();
    void Init(GLFWindow *window);
    void Update();
    void WaitIdle();
  public:
    vk::raii::Instance& GetInstance() { return m_Instance; };
  private:
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface(GLFWindow* window);
  private:
    vk::raii::Context m_Context;
    vk::raii::Instance m_Instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    vk::raii::SurfaceKHR m_Surface = nullptr;
  private:
    VulkanDevice m_Device;
    VulkanSwapChain m_Swapchain;
    VulkanGraphicsPipeline m_GraphicsPipeline;
    VulkanRenderer m_Renderer;
};
