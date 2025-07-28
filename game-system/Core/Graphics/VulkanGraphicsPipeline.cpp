#include "VulkanGraphicsPipeline.h"
#include <fstream>
#include "Core/Logging/Logger.h"


void VulkanGraphicsPipeline::CreateGraphicsPipeline(vk::raii::Device& device, const vk::Format* format)
{
    vk::raii::ShaderModule shaderModule = CreateShaderModule(ReadFile("Assets/Shaders/slang.spv"), device);
    vk::PipelineShaderStageCreateInfo vertShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eVertex, .module = shaderModule,  .pName = "vertMain" };
    vk::PipelineShaderStageCreateInfo fragShaderStageInfo{ .stage = vk::ShaderStageFlagBits::eFragment, .module = shaderModule, .pName = "fragMain" };
    vk::PipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{  .topology = vk::PrimitiveTopology::eTriangleList };
    vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .scissorCount = 1 };

    vk::PipelineRasterizationStateCreateInfo rasterizer{  .depthClampEnable = vk::False, .rasterizerDiscardEnable = vk::False,
                                                            .polygonMode = vk::PolygonMode::eFill, .cullMode = vk::CullModeFlagBits::eBack,
                                                            .frontFace = vk::FrontFace::eClockwise, .depthBiasEnable = vk::False,
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

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 0, .pushConstantRangeCount = 0 };

    m_PipelineLayout = vk::raii::PipelineLayout( device, pipelineLayoutInfo );
    CORE_LOG_INFO("Pipeline layout created.");

    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{ .colorAttachmentCount = 1, .pColorAttachmentFormats = format };
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

    m_GraphicsPipeline = vk::raii::Pipeline(device, nullptr, pipelineInfo);
    CORE_LOG_INFO("Graphics pipeline created.");
}

[[nodiscard]] vk::raii::ShaderModule VulkanGraphicsPipeline::CreateShaderModule(const std::vector<char>& code,  vk::raii::Device& device) const
{
    vk::ShaderModuleCreateInfo createInfo{ .codeSize = code.size() * sizeof(char), .pCode = reinterpret_cast<const uint32_t*>(code.data()) };
    vk::raii::ShaderModule shaderModule{ device, createInfo };
    CORE_LOG_INFO("Shadermodule created.");
    return shaderModule;
}

std::vector<char> VulkanGraphicsPipeline::ReadFile(const std::string& filename) {
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
