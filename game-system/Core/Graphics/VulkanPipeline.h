#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp> 
#include <vector>

class VulkanContext;

class VulkanPipeline
{
  public:
    VulkanPipeline(VulkanContext* context);
    ~VulkanPipeline();
    void CreateGraphicsPipeline();
    VkShaderModule CreateShaderModule(const std::vector<char> &code);
  private:  
    std::vector<char> VertexShaderCode;
    std::vector<char> FragmentShaderCode;
  private:
    VulkanContext *p_Context;
};