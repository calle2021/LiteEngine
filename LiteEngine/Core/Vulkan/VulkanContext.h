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
    VulkanContext(GLFWindow& window);
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
    VulkanDevice m_VulkanDevice;
    VulkanSwapChain m_VulkanSwapChain;
    VulkanGraphicsPipeline m_VulkanGraphicsPipeline;
    VulkanRenderer m_VulkanRenderer;
};
