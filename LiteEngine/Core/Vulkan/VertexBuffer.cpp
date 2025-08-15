#include "VertexBuffer.h"
#include "Core/Logging/Logger.h"

namespace LiteVulkan {
const std::vector<VertexBuffer::Vertex> m_Verticies = {
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
};

VertexBuffer::VertexBuffer(Device& device)
    : m_Device(device) {}

void VertexBuffer::CreateVertexBuffer()
{
    vk::BufferCreateInfo bufferInfo {
        .size = sizeof(m_Verticies[0]) * m_Verticies.size(),
        .usage = vk::BufferUsageFlagBits::eVertexBuffer,
        .sharingMode = vk::SharingMode::eExclusive
    };
    m_VertexBuffer = vk::raii::Buffer(m_Device.m_Device, bufferInfo);
    CORE_LOG_INFO("VertexBuffer created");
    vk::MemoryRequirements memRequirements = m_VertexBuffer.getMemoryRequirements();
    vk::MemoryAllocateInfo memoryAllocateInfo {
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = FindMemoryType(
            memRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        ),
    };
    m_VertexBufferMemory = vk::raii::DeviceMemory(m_Device.m_Device, memoryAllocateInfo);

    CORE_LOG_INFO("VertexBufferMemory created");

    m_VertexBuffer.bindMemory( *m_VertexBufferMemory, 0 );

    void* data = m_VertexBufferMemory.mapMemory(0, bufferInfo.size);
    memcpy(data, m_Verticies.data(), bufferInfo.size);
    m_VertexBufferMemory.unmapMemory();
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