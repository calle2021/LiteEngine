#include "VertexBuffer.h"
#include "Core/Logging/Logger.h"

namespace LiteVulkan {
const std::vector<VertexBuffer::Vertex> m_Verticies = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
};

VertexBuffer::VertexBuffer(Device& device, Renderer& rend)
    : m_Device(device)
    , m_Renderer(rend) {}

void VertexBuffer::CreateVertexBuffer()
{
    vk::BufferCreateInfo stagingInfo{
        .size = sizeof(m_Verticies[0]) * m_Verticies.size(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .sharingMode = vk::SharingMode::eExclusive
    };
    vk::raii::Buffer stagingBuffer(m_Device.m_Device, stagingInfo);
    vk::MemoryRequirements memRequirementsStaging = stagingBuffer.getMemoryRequirements();
    vk::MemoryAllocateInfo memoryAllocateInfoStaging{
        .allocationSize = memRequirementsStaging.size,
        .memoryTypeIndex = FindMemoryType(
                                          memRequirementsStaging.memoryTypeBits,
                                          vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
                                        ),
    };
    vk::raii::DeviceMemory stagingBufferMemory(m_Device.m_Device, memoryAllocateInfoStaging);

    stagingBuffer.bindMemory(stagingBufferMemory, 0);
    void* dataStaging = stagingBufferMemory.mapMemory(0, stagingInfo.size);
    memcpy(dataStaging, m_Verticies.data(), stagingInfo.size);
    stagingBufferMemory.unmapMemory();

    vk::BufferCreateInfo bufferInfo {
        .size = sizeof(m_Verticies[0]) * m_Verticies.size(),
        .usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        .sharingMode = vk::SharingMode::eExclusive
    };
    m_VertexBuffer = vk::raii::Buffer(m_Device.m_Device, bufferInfo);
    vk::MemoryRequirements memRequirements = m_VertexBuffer.getMemoryRequirements();
    vk::MemoryAllocateInfo memoryAllocateInfo {
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = FindMemoryType(
            memRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eDeviceLocal
        ),
    };
    m_VertexBufferMemory = vk::raii::DeviceMemory(m_Device.m_Device, memoryAllocateInfo);
    m_VertexBuffer.bindMemory( *m_VertexBufferMemory, 0 );
    CopyBuffer(stagingBuffer, m_VertexBuffer, stagingInfo.size);
    CORE_LOG_INFO("Create VertexBuffer");
}

void VertexBuffer::CopyBuffer(vk::raii::Buffer& src, vk::raii::Buffer& dst, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = m_Renderer.m_CommandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    vk::raii::CommandBuffer commandCopyBuffer = std::move(m_Device.m_Device.allocateCommandBuffers(allocInfo).front());
    commandCopyBuffer.begin(vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    commandCopyBuffer.copyBuffer(*src, *dst, vk::BufferCopy(0, 0, size));
    commandCopyBuffer.end();
    m_Device.m_Queue.submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer }, nullptr);
    m_Device.m_Queue.waitIdle();
}


uint32_t VertexBuffer::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = m_Device.m_PhysicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    CORE_LOG_ERROR("Failed to find suitable memory type");
    throw std::runtime_error("failed to find suitable memory type");
}
}