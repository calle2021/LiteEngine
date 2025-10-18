
#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "SwapChain.h"
#include "Device.h"
#include "Buffers.h"
#include "Assets.h"

namespace LiteVulkan {
class Renderer;
class Pipeline
{
friend class Renderer;
public:
    Pipeline(SwapChain& swapChain, const Device& device, Buffers& buf, Assets& assets);
    void CreateDescriptorLayout();
    void CreatePipeline();
    void CreateDescriptorSets();
    void CreateDescriptorPool();
    const vk::raii::Pipeline& GetPipeline() const { return m_Pipeline; }
private:
    vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code) const;
    static std::vector<char> ReadFile(const std::string& filename);
private:
    vk::raii::DescriptorSetLayout m_DescriptorLayout = nullptr;
    vk::raii::DescriptorPool m_DescriptorPool = nullptr;
    std::vector<vk::raii::DescriptorSet> m_DescriptorSets;
    vk::raii::PipelineLayout m_PipelineLayout = nullptr;
    vk::raii::Pipeline m_Pipeline = nullptr;
private:
    SwapChain& m_SwapChainRef;
    const Device& m_DeviceRef;
    Buffers& m_BuffersRef;
    Assets& m_AssetsRef;
};
}