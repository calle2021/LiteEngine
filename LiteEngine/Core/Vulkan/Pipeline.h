
#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "SwapChain.h"
#include "Device.h"

namespace LiteVulkan {
class Renderer;
class Pipeline
{
friend class Renderer;
public:
    Pipeline(SwapChain& swapChain, Device& device);
    void CreateDescriptorLayout();
    void CreatePipeline();
    const vk::raii::Pipeline& GetPipeline() const { return m_Pipeline; }
private:
    [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code, vk::raii::Device& device) const;
    static std::vector<char> ReadFile(const std::string& filename);
private:
    vk::raii::DescriptorSetLayout m_DescriptorLayout = nullptr;
    vk::raii::PipelineLayout m_PipelineLayout = nullptr;
    vk::raii::Pipeline m_Pipeline = nullptr;
private:
    SwapChain& m_SwapChainRef;
    Device& m_DeviceRef;
};
}