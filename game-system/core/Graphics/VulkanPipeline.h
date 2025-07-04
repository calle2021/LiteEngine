#pragma once
#include <vulkan/vulkan.h>

#include <vector>

class VulkanPipeline
{
  public:
    VulkanPipeline();
    ~VulkanPipeline();
    void LoadShaders();

  private:
    VkShaderModule CreateShaderModule(const std::vector<char> &code);
};