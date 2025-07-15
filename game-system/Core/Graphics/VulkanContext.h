#pragma once
#include <vulkan/vulkan.hpp>

#include "Core/Window/GLFWindow.h"

class VulkanContext
{
  public:
    VulkanContext();
    ~VulkanContext();

  public:
    void Init(Window *window);
  private:
    void CreateInstance();
};
