#include "Buffers.h"
#include "Core/Logging/Logger.h"
#include "Config.h"

namespace LiteVulkan {
const std::vector<Buffers::Vertex> m_Verticies = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

Buffers::Buffers(Device& dev, Renderer& rend, SwapChain& swap)
    : m_DeviceRef(dev)
    , m_RendererRef(rend)
    , m_SwapChainRef(swap) {}

void Buffers::CreateBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory) {
    vk::BufferCreateInfo bufferInfo{ .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };
    buffer = vk::raii::Buffer(m_DeviceRef.m_Device, bufferInfo);
    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{ .allocationSize =memRequirements.size, .memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties) };
    bufferMemory = vk::raii::DeviceMemory(m_DeviceRef.m_Device, allocInfo);
    buffer.bindMemory(bufferMemory, 0);
}

void Buffers::CreateVertexBuffer()
{
    vk::DeviceSize bufferSize = sizeof(m_Verticies[0]) * m_Verticies.size();
    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});
    CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(dataStaging, m_Verticies.data(), bufferSize);
    stagingBufferMemory.unmapMemory();

    CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, m_Buffers, m_BuffersMemory);

    CopyBuffer(stagingBuffer, m_Buffers, bufferSize);
    CORE_LOG_INFO("Create VertexBuffer");
}

void Buffers::CreateIndexBuffer()
{
     vk::DeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});
    CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, m_Indices.data(), (size_t) bufferSize);
    stagingBufferMemory.unmapMemory();

    CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, m_IndexBuffer, m_IndexBufferMemory);

    CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);
}

void Buffers::CreateUniformBuffers()
{
    m_UniformBuffers.clear();
    m_UniformBuffersMemory.clear();
    m_UniformBuffersMapped.clear();

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
        vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
        vk::raii::Buffer buffer({});
        vk::raii::DeviceMemory bufferMem({});
        CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, buffer, bufferMem);
        m_UniformBuffers.emplace_back(std::move(buffer));
        m_UniformBuffersMemory.emplace_back(std::move(bufferMem));
        m_UniformBuffersMapped.emplace_back( m_UniformBuffersMemory[i].mapMemory(0, bufferSize));
    }
}

void Buffers::UpdateUniformBuffer(uint32_t curr)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(m_SwapChainRef.m_Extent.width) / static_cast<float>(m_SwapChainRef.m_Extent.height), 0.1f, 10.0f);
    ubo.proj[1][1] *= -1;

    memcpy(m_UniformBuffersMapped[curr], &ubo, sizeof(ubo));
}

void Buffers::CopyBuffer(vk::raii::Buffer& src, vk::raii::Buffer& dst, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = m_RendererRef.m_CommandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    vk::raii::CommandBuffer commandCopyBuffer = std::move(m_DeviceRef.m_Device.allocateCommandBuffers(allocInfo).front());
    commandCopyBuffer.begin(vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    commandCopyBuffer.copyBuffer(*src, *dst, vk::BufferCopy{ .size = size });
    commandCopyBuffer.end();
    m_DeviceRef.m_Queue.submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer }, nullptr);
    m_DeviceRef.m_Queue.waitIdle();
}


uint32_t Buffers::FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = m_DeviceRef.m_PhysicalDevice.getMemoryProperties();

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    CORE_LOG_ERROR("Failed to find suitable memory type");
    throw std::runtime_error("failed to find suitable memory type");
}
}