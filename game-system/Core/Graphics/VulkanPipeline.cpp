#include "VulkanPipeline.h"
#include "VulkanContext.h"
#include "VulkanBase.h"
#include "pch.h"

VulkanPipeline::VulkanPipeline(VulkanContext* context) : p_Context(context) {};
VulkanPipeline::~VulkanPipeline(){};

void VulkanPipeline::CreateGraphicsPipeline()
{
    VertexShaderCode = p_Context->c_Base.ReadFile("Assets/Shaders/vert.spv");
    FragmentShaderCode = p_Context->c_Base.ReadFile("Assets/Shaders/frag.spv");
    VkShaderModule ShaderModule = CreateShaderModule(VertexShaderCode);
    VkPipelineShaderStageCreateInfo VertexShaderStageInfo {};
    VertexShaderStageInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
    VertexShaderStageInfo.module = ShaderModule,
    VertexShaderStageInfo.pName = "vertMain";
}

VkShaderModule VulkanPipeline::CreateShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo CreatInfo{};
    CreatInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreatInfo.codeSize = code.size();
    CreatInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    VkShaderModule ShaderModule;
    if (vkCreateShaderModule(p_Context->m_Device, &CreatInfo, nullptr, &ShaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module");
    }
    return ShaderModule;
}