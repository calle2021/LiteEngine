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
    VulkanContext();
    ~VulkanContext();

  public:
    void Init(Window *window);
  public:
    vk::raii::Instance& GetInstance() { return m_Instance; };
  private:
    void CreateInstance();
  public:
    Window* m_Window;
  private:
    vk::raii::Context m_Context;
    vk::raii::Instance m_Instance;
  private:
    VulkanDevice m_Device;
};
