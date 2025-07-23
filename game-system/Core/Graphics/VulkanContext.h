#pragma once
#include <memory>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_platform.h>
#include "Core/Window/GLFWindow.h"
#include "VulkanDevice.h"

class VulkanDevice;

class VulkanContext
{
  public:
    void Init(GLFWindow *window);
  public:
    vk::raii::Instance& GetInstance() { return m_Instance; };
  private:
    void CreateInstance();
    void SetupDebugMessenger();
  public:
    GLFWindow* m_Window;
  private:
    vk::raii::Context m_Context;
    vk::raii::Instance m_Instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
  private:
    VulkanDevice m_Device;
};
