
#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "SwapChain.h"
#include "Device.h"

namespace LiteVulkan {
class Renderer;
class GraphicsPipeline
{
friend class Renderer;
public:
    GraphicsPipeline(SwapChain& swapChain, Device& device);
    void CreateGraphicsPipeline();
    const vk::raii::Pipeline& GetGraphicsPipeline() const { return m_GraphicsPipeline; }
private:
    [[nodiscard]] vk::raii::ShaderModule CreateShaderModule(const std::vector<char>& code, vk::raii::Device& device) const;
    static std::vector<char> ReadFile(const std::string& filename);
private:
    vk::raii::PipelineLayout m_PipelineLayout = nullptr;
    vk::raii::Pipeline m_GraphicsPipeline = nullptr;
private:
    SwapChain& m_SwapChain;
    Device& m_Device;
};
}