#pragma once
#include <vector>

class VulkanContext;

/* Vulkan helper class */
class VulkanBase
{
  public:
    VulkanBase(VulkanContext *context);
    ~VulkanBase();

  public:
    std::vector<const char *> GetRequiredExtenstions();

  private:
    VulkanContext *p_Context;
};