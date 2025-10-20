
#pragma once
#include "Device.h"
#include <vulkan/vulkan_raii.hpp>

namespace LiteVulkan {
class Renderer;
class Pipeline
{
    friend class Renderer;

public:
    Pipeline(const Device& device);
    void CreateDescriptorLayout();
    void CreatePipeline(const vk::Format imageFormat, const vk::Format depthFormat);
    void CreateDescriptorSets(const uint32_t nFramesInFlight,
                              const vk::raii::Sampler& textureSampler,
                              const vk::raii::ImageView& textureImageView,
                              const std::vector<vk::raii::Buffer>& uniformBuffer);
    void CreateDescriptorPool(const uint32_t nFramesInFlight);
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
    const Device& m_DeviceRef;
};
}