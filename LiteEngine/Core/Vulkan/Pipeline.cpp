#include "Pipeline.h"
#include "Buffers.h"
#include "Core/Logging/Logger.h"
#include "Vertex.h"
#include <fstream>

namespace LiteVulkan {
Pipeline::Pipeline(const Device& device)
    : m_DeviceRef(device)
{
}

void Pipeline::CreateDescriptorLayout()
{
    auto uniBufferBinding = vk::DescriptorSetLayoutBinding(
        0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr);
    auto samplerBinding
        = vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1,
                                         vk::ShaderStageFlagBits::eFragment, nullptr);
    std::array descBindings = { uniBufferBinding, samplerBinding };
    vk::DescriptorSetLayoutCreateInfo descLayout = {};
    descLayout.bindingCount = descBindings.size();
    descLayout.pBindings = descBindings.data();
    m_DescriptorLayout = vk::raii::DescriptorSetLayout(m_DeviceRef.GetDevice(), descLayout);
    CORE_LOG_INFO("Descriptor layout created");
}

void Pipeline::CreateDescriptorSets(const uint32_t MAX_FRAMES_IN_FLIGHT,
                                    const vk::raii::Sampler& textureSampler,
                                    const vk::raii::ImageView& textureImageView,
                                    const std::vector<vk::raii::Buffer>& uniformBuffer)
{
    std::vector<vk::DescriptorSetLayout> descLayouts(MAX_FRAMES_IN_FLIGHT, m_DescriptorLayout);
    vk::DescriptorSetAllocateInfo descAlloc = {};
    descAlloc.descriptorPool = m_DescriptorPool;
    descAlloc.descriptorSetCount = static_cast<uint32_t>(descLayouts.size());
    descAlloc.pSetLayouts = descLayouts.data();

    m_DescriptorSets.clear();
    m_DescriptorSets = m_DeviceRef.GetDevice().allocateDescriptorSets(descAlloc);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorBufferInfo descBuffer = {};
        descBuffer.buffer = uniformBuffer[i];
        descBuffer.offset = 0;
        descBuffer.range = sizeof(Buffers::UniformBufferObject);

        vk::DescriptorImageInfo descImage = {};
        descImage.sampler = textureSampler;
        descImage.imageView = textureImageView;
        descImage.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

        vk::WriteDescriptorSet uniWriteDesc = {};
        uniWriteDesc.dstSet = m_DescriptorSets[i];
        uniWriteDesc.dstBinding = 0;
        uniWriteDesc.dstArrayElement = 0;
        uniWriteDesc.descriptorCount = 1;
        uniWriteDesc.descriptorType = vk::DescriptorType::eUniformBuffer;
        uniWriteDesc.pBufferInfo = &descBuffer;

        vk::WriteDescriptorSet samplerWriteDesc = {};
        samplerWriteDesc.dstSet = m_DescriptorSets[i];
        samplerWriteDesc.dstBinding = 1;
        samplerWriteDesc.dstArrayElement = 0;
        samplerWriteDesc.descriptorCount = 1;
        samplerWriteDesc.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        samplerWriteDesc.pImageInfo = &descImage;

        std::array descWrites = { uniWriteDesc, samplerWriteDesc };
        m_DeviceRef.GetDevice().updateDescriptorSets(descWrites, {});
    }
    CORE_LOG_INFO("Descriptor sets created");
}
void Pipeline::CreateDescriptorPool(const uint32_t MAX_FRAMES_IN_FLIGHT)
{
    auto uniDescPool
        = vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, MAX_FRAMES_IN_FLIGHT);
    auto samplerDescPool
        = vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, MAX_FRAMES_IN_FLIGHT);
    std::array descPools = { uniDescPool, samplerDescPool };

    vk::DescriptorPoolCreateInfo descPool = {};
    descPool.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    descPool.maxSets = MAX_FRAMES_IN_FLIGHT;
    descPool.poolSizeCount = static_cast<uint32_t>(descPools.size());
    descPool.pPoolSizes = descPools.data();
    m_DescriptorPool = vk::raii::DescriptorPool(m_DeviceRef.GetDevice(), descPool);
    CORE_LOG_INFO("Descriptor pool created");
}

void Pipeline::CreatePipeline(const vk::Format imageFormat, const vk::Format depthFormat)
{
    vk::raii::ShaderModule shaderModule = CreateShaderModule(ReadFile("Assets/Shaders/slang.spv"));

    vk::PipelineShaderStageCreateInfo vertShaderStage = {};
    vertShaderStage.stage = vk::ShaderStageFlagBits::eVertex;
    vertShaderStage.module = shaderModule;
    vertShaderStage.pName = "vertMain";

    vk::PipelineShaderStageCreateInfo fragShaderStage = {};
    fragShaderStage.stage = vk::ShaderStageFlagBits::eFragment;
    fragShaderStage.module = shaderModule;
    fragShaderStage.pName = "fragMain";

    vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStage, fragShaderStage };

    auto vertBindDesc = Vertex::GetBindingDescription();
    auto vertAttribDesc = Vertex::GetAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInfo = {};
    vertexInfo.vertexBindingDescriptionCount = 1;
    vertexInfo.pVertexBindingDescriptions = &vertBindDesc;
    vertexInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertAttribDesc.size());
    vertexInfo.pVertexAttributeDescriptions = vertAttribDesc.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = vk::False;

    vk::PipelineViewportStateCreateInfo viewportState = {};
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    vk::PipelineRasterizationStateCreateInfo rasterizerState = {};
    rasterizerState.depthClampEnable = vk::False;
    rasterizerState.rasterizerDiscardEnable = vk::False;
    rasterizerState.polygonMode = vk::PolygonMode::eFill;
    rasterizerState.cullMode = vk::CullModeFlagBits::eBack;
    rasterizerState.frontFace = vk::FrontFace::eCounterClockwise;
    rasterizerState.depthBiasEnable = vk::False;
    rasterizerState.lineWidth = 1;

    // sampleShadingEnable and minSampleShading = 1.0f might be costly
    vk::PipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.rasterizationSamples = m_DeviceRef.GetMsaaSamples();
    multisampling.sampleShadingEnable = vk::True;
    multisampling.minSampleShading = 1.0f;

    vk::PipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.depthTestEnable = vk::True;
    depthStencil.depthWriteEnable = vk::True;
    depthStencil.depthCompareOp = vk::CompareOp::eLess;
    depthStencil.depthBoundsTestEnable = vk::False;
    depthStencil.stencilTestEnable = vk::False;

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = vk::False;

    vk::PipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.logicOpEnable = vk::False;
    colorBlending.logicOp = vk::LogicOp::eCopy;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    std::vector dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
    vk::PipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    vk::PipelineLayoutCreateInfo pipeLayout = {};
    pipeLayout.setLayoutCount = 1;
    pipeLayout.pSetLayouts = &*m_DescriptorLayout;
    pipeLayout.pushConstantRangeCount = 0;

    m_PipelineLayout = vk::raii::PipelineLayout(m_DeviceRef.GetDevice(), pipeLayout);

    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo = {};
    pipelineRenderingCreateInfo.colorAttachmentCount = 1;
    pipelineRenderingCreateInfo.pColorAttachmentFormats = &imageFormat;
    pipelineRenderingCreateInfo.depthAttachmentFormat = depthFormat;

    vk::GraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.pNext = &pipelineRenderingCreateInfo;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizerState;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_PipelineLayout;
    pipelineInfo.renderPass = nullptr;

    m_Pipeline = vk::raii::Pipeline(m_DeviceRef.GetDevice(), nullptr, pipelineInfo);
    CORE_LOG_INFO("Pipeline created");
}

vk::raii::ShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code) const
{
    vk::ShaderModuleCreateInfo shaderInfo = {};
    shaderInfo.codeSize = code.size() * sizeof(char);
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    vk::raii::ShaderModule shaderModule = { m_DeviceRef.GetDevice(), shaderInfo };
    CORE_LOG_INFO("Shadermodule created");
    return shaderModule;
}

std::vector<char> Pipeline::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file!");
    }
    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();
    CORE_LOG_INFO("File {} loaded", filename);
    return buffer;
}
}