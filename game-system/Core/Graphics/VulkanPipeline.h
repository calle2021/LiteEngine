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
  public:
    void CreateRenderPass();
    void CreateGraphicsPipeline();
    void Destroy();
  private:
    VkShaderModule CreateShaderModule(const std::vector<char> &code);
  private:
    VkRenderPass m_RenderPass;
    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_GraphicsPipeline;
    VulkanContext *p_Context;
};