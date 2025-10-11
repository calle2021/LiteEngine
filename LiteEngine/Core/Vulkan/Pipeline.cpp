#include "Pipeline.h"
#include <fstream>
#include "Core/Logging/Logger.h"
#include "Buffers.h"
#include "Config.h"

namespace LiteVulkan {
Pipeline::Pipeline(SwapChain& swap, Device& dev, Buffers& buf, Assets& assets)
    : m_SwapChainRef(swap)
    , m_DeviceRef(dev)
    , m_BuffersRef(buf)
    , m_AssetsRef(assets) {}

void Pipeline::CreateDescriptorLayout()
{
    std::array bindings =
    {
        vk::DescriptorSetLayoutBinding( 0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),
        vk::DescriptorSetLayoutBinding( 1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr)
    };
    vk::DescriptorSetLayoutCreateInfo layoutInfo{.bindingCount = bindings.size(), .pBindings = bindings.data()};
    m_DescriptorLayout = vk::raii::DescriptorSetLayout(m_DeviceRef.m_Device, layoutInfo);
}

void Pipeline::CreateDescriptorSets()
{
    std::vector<vk::DescriptorSetLayout> layouts(FRAMES_IN_FLIGHT, m_DescriptorLayout);
    vk::DescriptorSetAllocateInfo allocInfo{ .descriptorPool = m_DescriptorPool, .descriptorSetCount = static_cast<uint32_t>(layouts.size()), .pSetLayouts = layouts.data() };

    m_DescriptorSets.clear();
    m_DescriptorSets = m_DeviceRef.m_Device.allocateDescriptorSets(allocInfo);

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        vk::DescriptorBufferInfo bufferInfo{ .buffer = m_BuffersRef.m_UniformBuffers[i], .offset = 0, .range = sizeof(Buffers::UniformBufferObject) };
        vk::DescriptorImageInfo imageInfo{
            .sampler = m_AssetsRef.m_TextureSampler,
            .imageView = m_AssetsRef.m_TextureImageView,
            .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
        };
        std::array descriptorWrites{
            vk::WriteDescriptorSet{
                .dstSet = m_DescriptorSets[i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .pBufferInfo = &bufferInfo
            },
            vk::WriteDescriptorSet{
                .dstSet = m_DescriptorSets[i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler,
                .pImageInfo = &imageInfo
            }
        };
        m_DeviceRef.m_Device.updateDescriptorSets(descriptorWrites, {});
    }
}
void Pipeline::CreateDescriptorPool()
{
    std::array pool {
        vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, FRAMES_IN_FLIGHT),
        vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, FRAMES_IN_FLIGHT)
    };
    vk::DescriptorPoolCreateInfo poolInfo{
        .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        .maxSets = FRAMES_IN_FLIGHT,
        .poolSizeCount = static_cast<uint32_t>(pool.size()),
        .pPoolSizes = pool.data()
    };
    m_DescriptorPool = vk::raii::DescriptorPool(m_DeviceRef.m_Device, poolInfo);
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
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &bindingDescription,
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size()),
        .pVertexAttributeDescriptions = attributeDescriptions.data()
    };

    vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList, .primitiveRestartEnable = vk::False };
    vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .scissorCount = 1 };

    vk::PipelineRasterizationStateCreateInfo rasterizer{  .depthClampEnable = vk::False, .rasterizerDiscardEnable = vk::False,
                                                            .polygonMode = vk::PolygonMode::eFill, .cullMode = vk::CullModeFlagBits::eBack,
                                                            .frontFace = vk::FrontFace::eCounterClockwise, .depthBiasEnable = vk::False };
    rasterizer.lineWidth = 1;
    // sampleShadingEnable and minSampleShading = 1.0f might be costly
    vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = m_DeviceRef.m_msaa_samples, .sampleShadingEnable = vk::True, .minSampleShading = 1.0f};

    vk::PipelineDepthStencilStateCreateInfo depthStencil{
        .depthTestEnable = vk::True,
        .depthWriteEnable = vk::True,
        .depthCompareOp = vk::CompareOp::eLess,
        .depthBoundsTestEnable = vk::False,
        .stencilTestEnable = vk::False
    };

    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = vk::False;

    vk::PipelineColorBlendStateCreateInfo colorBlending{
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment
    };

    std::vector dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };
    vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{ .setLayoutCount = 1, .pSetLayouts = &*m_DescriptorLayout, .pushConstantRangeCount = 0 };

    m_PipelineLayout = vk::raii::PipelineLayout( m_DeviceRef.m_Device, pipelineLayoutInfo );
    CORE_LOG_INFO("Pipeline layout created.");

    vk::Format depthFormat = m_AssetsRef.FindDepthFormat();
    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo { .colorAttachmentCount = 1, .pColorAttachmentFormats = &m_SwapChainRef.m_ImageFormat,
                                                                  .depthAttachmentFormat = depthFormat };
    vk::GraphicsPipelineCreateInfo pipelineInfo {
        .pNext = &pipelineRenderingCreateInfo,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil,
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