#include "VulkanPipeline.h"

#include "pch.h"

VulkanPipeline::VulkanPipeline(){};
VulkanPipeline::~VulkanPipeline(){};
void VulkanPipeline::LoadShaders()
{
    // auto vertShaderCode =
    // GameSystem::VulkanUtils::readFile("assets/shaders/vert.spv"); auto
    // fragShaderCode =
    // GameSystem::VulkanUtils::readFile("assets/shaders/frag.spv");
}

VkShaderModule VulkanPipeline::CreateShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo CreatInfo{};
    CreatInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreatInfo.codeSize = code.size();
    CreatInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());
    VkShaderModule ShaderModule;
    // if (vkCreateShaderModule(device, &CreatInfo, nullptr, &ShaderModule) !=
    // VK_SUCCESS) {
    //     CORE_ERROR("Failed to create shader module");
    //     throw std::runtime_error("failed to create shader module!");
    // }
    return ShaderModule;
}