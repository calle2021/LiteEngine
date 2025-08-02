
#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "VulkanSwapChain.h"
#include "VulkanDevice.h"

class VulkanRenderer;

class VulkanGraphicsPipeline
{
friend class VulkanRenderer;
public:
    VulkanGraphicsPipeline(VulkanSwapChain& swapChain, VulkanDevice& device);
    void CreateGraphicsPipeline();
    const vk::raii::Pipeline& GetGraphicsPipeline() const { return m_GraphicsPipeline; }
private:
    [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code, vk::raii::Device& device) const;
    static std::vector<char> ReadFile(const std::string& filename);
private:
    vk::raii::PipelineLayout m_PipelineLayout = nullptr;
    vk::raii::Pipeline m_GraphicsPipeline = nullptr;
private:
    VulkanSwapChain& m_VulkanSwapChain;
    VulkanDevice& m_VulkanDevice;
};
