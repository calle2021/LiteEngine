#include "Pipeline.h"
#include <fstream>
#include "Core/Logging/Logger.h"
#include "Buffers.h"

namespace LiteVulkan {
Pipeline::Pipeline(SwapChain& swap, Device& dev)
    : m_SwapChainRef(swap)
    , m_DeviceRef(dev) {}

void Pipeline::CreateDescriptorLayout()
{
    vk::DescriptorSetLayoutBinding uboLayoutBinding(0, vk::DescriptorType::eUniformBuffer,
                                                    1, vk::ShaderStageFlagBits::eVertex, nullptr);
    vk::DescriptorSetLayoutCreateInfo layoutInfo{ .bindingCount = 1, .pBindings = &uboLayoutBinding };
    m_DescriptorLayout = vk::raii::DescriptorSetLayout(m_DeviceRef.m_Device, layoutInfo);
}

void Pipeline::CreatePipeline()
{
    vk::raii::ShaderModule shaderModule = CreateShaderModule(ReadFile("Assets/Shaders/slang.spv"), m_DeviceRef.m_Device);
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule,  .pName = "vertMain" };
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };
    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    auto bindingDescription = Buffers::Vertex::GetBindingDescription();
    auto attributeDescriptions = Buffers::Vertex::GetAttributeDescriptions();
    vk::PipelineVertexInputStateCreateInfo vertexInputInfo {
        .vertexBindingDescriptionCount =1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{  .topology = vk::PrimitiveTopology::eTriangleList };
    vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .scissorCount = 1 };

    vk::PipelineRasterizationStateCreateInfo rasterizer{  .depthClampEnable = vk::False, .rasterizerDiscardEnable = vk::False,
                                                            .polygonMode = vk::PolygonMode::eFill, .cullMode = vk::CullModeFlagBits::eBack,
                                                            .frontFace = vk::FrontFace::eCounterClockwise, .depthBiasEnable = vk::False,
                                                            .depthBiasSlopeFactor = 1.0f, .lineWidth = 1.0f };

    vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False};

    vk::PipelineColorBlendAttachmentState colorBlendAttachment{ .blendEnable = vk::False,
        .colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
    };

    vk::PipelineColorBlendStateCreateInfo colorBlending{ .logicOpEnable = vk::False, .logicOp =  vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments =  &colorBlendAttachment };

    std::vector dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };
    vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 1, .pSetLayouts = &*m_DescriptorLayout, .pushConstantRangeCount = 0 };

    m_PipelineLayout = vk::raii::PipelineLayout( m_DeviceRef.m_Device, pipelineLayoutInfo );
    CORE_LOG_INFO("Pipeline layout created.");

    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{ .colorAttachmentCount = 1, .pColorAttachmentFormats = &m_SwapChainRef.m_ImageFormat };
    vk::GraphicsPipelineCreateInfo pipelineInfo {
        .pNext = &pipelineRenderingCreateInfo,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = m_PipelineLayout,
        .renderPass = nullptr
    };

    m_Pipeline = vk::raii::Pipeline(m_DeviceRef.m_Device, nullptr, pipelineInfo);
    CORE_LOG_INFO("Graphics pipeline created.");
}

[[nodiscard]] vk::raii::ShaderModule Pipeline::CreateShaderModule(const std::vector<char>& code,  vk::raii::Device& device) const
{
    vk::ShaderModuleCreateInfo createInfo{ .codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(code.data()) };
    vk::raii::ShaderModule shaderModule{ m_DeviceRef.m_Device, createInfo };
    CORE_LOG_INFO("Shadermodule created.");
    return shaderModule;
}

std::vector<char> Pipeline::ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        CORE_LOG_ERROR("Failed to open {}", filename);
        throw std::runtime_error("Failed to open file.");
    }
    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();
    CORE_LOG_INFO("File {} loaded.", filename);
    return buffer;
}
}