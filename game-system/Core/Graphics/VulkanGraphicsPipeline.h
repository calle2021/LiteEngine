
#pragma once
#include <vulkan/vulkan_raii.hpp>
class VulkanRenderer;

class VulkanGraphicsPipeline
{
friend class VulkanRenderer;
public:
    void CreateGraphicsPipeline(vk::raii::Device& device, const vk::Format& format);
    const vk::raii::Pipeline& GetGraphicsPipeline() const { return m_GraphicsPipeline; }
private:
    [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code, vk::raii::Device& device) const;
    static std::vector<char> ReadFile(const std::string& filename);
private:
    vk::raii::PipelineLayout m_PipelineLayout = nullptr;
    vk::raii::Pipeline m_GraphicsPipeline = nullptr;
};
