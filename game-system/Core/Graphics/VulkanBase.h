#pragma once
#include <vector>
#include <string>

class VulkanContext;

/* Vulkan helper class */
class VulkanBase
{
  public:
    VulkanBase(VulkanContext *context);
    ~VulkanBase();

  public:
    std::vector<const char *> GetRequiredExtenstions();
    std::vector<char> ReadFile(const std::string& filename);

  private:
    VulkanContext *p_Context;
};