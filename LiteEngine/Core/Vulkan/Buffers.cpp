#include "Buffers.h"
#include "Core/Logging/Logger.h"
#include "Config.h"
#include <unordered_map>

namespace LiteVulkan {
Buffers::Buffers(const Device& device, const LiteEngine::Camera& camera)
    : m_DeviceRef(device), m_CameraRef(camera) {}

void Buffers::CreateBuffer(const Device& device, vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory) {
    vk::BufferCreateInfo bufferInfo{ .size = size, .usage = usage, .sharingMode = vk::SharingMode::eExclusive };
    buffer = vk::raii::Buffer(device.GetDevice(), bufferInfo);
    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{ .allocationSize =memRequirements.size, .memoryTypeIndex = FindMemoryType(device.GetPhysicalDevice().getMemoryProperties(), memRequirements.memoryTypeBits, properties) };
    bufferMemory = vk::raii::DeviceMemory(device.GetDevice(), allocInfo);
    buffer.bindMemory(bufferMemory, 0);
}

void Buffers::CreateVertexBuffer(const vk::raii::CommandPool& command_pool)
{
    vk::DeviceSize bufferSize = sizeof(m_Verticies[0]) * m_Verticies.size();
    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});
    CreateBuffer(m_DeviceRef, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* dataStaging = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(dataStaging, m_Verticies.data(), bufferSize);
    stagingBufferMemory.unmapMemory();

    CreateBuffer(m_DeviceRef, bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, m_Buffers, m_BuffersMemory);

    CopyBuffer(command_pool, stagingBuffer, m_Buffers, bufferSize);
    CORE_LOG_INFO("Create VertexBuffer");
}

void Buffers::CreateIndexBuffer(const vk::raii::CommandPool& command_pool)
{
    vk::DeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});
    CreateBuffer(m_DeviceRef, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* data = stagingBufferMemory.mapMemory(0, bufferSize);
    memcpy(data, m_Indices.data(), (size_t) bufferSize);
    stagingBufferMemory.unmapMemory();

    CreateBuffer(m_DeviceRef, bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, m_IndexBuffer, m_IndexBufferMemory);

    CopyBuffer(command_pool, stagingBuffer, m_IndexBuffer, bufferSize);
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
        CreateBuffer(m_DeviceRef, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, buffer, bufferMem);
        m_UniformBuffers.emplace_back(std::move(buffer));
        m_UniformBuffersMemory.emplace_back(std::move(bufferMem));
        m_UniformBuffersMapped.emplace_back( m_UniformBuffersMemory[i].mapMemory(0, bufferSize));
    }
}

void Buffers::UpdateUniformBuffer(uint32_t curr)
{
    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
    ubo.view = m_CameraRef.GetView();
    ubo.proj = m_CameraRef.GetProjection();
    ubo.proj[1][1] *= -1;

    memcpy(m_UniformBuffersMapped[curr], &ubo, sizeof(ubo));
}

void Buffers::CopyBuffer(const vk::raii::CommandPool& command_pool, vk::raii::Buffer& src, vk::raii::Buffer& dst, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = command_pool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    vk::raii::CommandBuffer commandCopyBuffer = std::move(m_DeviceRef.GetDevice().allocateCommandBuffers(allocInfo).front());
    commandCopyBuffer.begin(vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    commandCopyBuffer.copyBuffer(*src, *dst, vk::BufferCopy{ .size = size });
    commandCopyBuffer.end();
    m_DeviceRef.GetQueue().submit(vk::SubmitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandCopyBuffer }, nullptr);
    m_DeviceRef.GetQueue().waitIdle();
}

void Buffers::BufferModels(const std::vector<tinyobj::shape_t>& shapes, const tinyobj::attrib_t& attrib)
{
    std::unordered_map<Vertex, uint32_t> unique_verticies;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.m_Pos = {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.m_Tex = {
                attrib.texcoords[2 * index.texcoord_index + 0],
                // obj format defines 0 as bottom but in vulkan 0 is top
                // so we flip the vertical component
                1.0 - attrib.texcoords[2 * index.texcoord_index + 1]
            };
            vertex.m_Color = {1.0f, 1.0f, 1.0f};

            if (unique_verticies.count(vertex) == 0) {
                unique_verticies[vertex] = static_cast<uint32_t>(m_Verticies.size());
                m_Verticies.push_back(vertex);
            }

            m_Indices.push_back(unique_verticies[vertex]);
        }
    }
}

uint32_t Buffers::FindMemoryType(vk::PhysicalDeviceMemoryProperties memProperties, const uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    CORE_LOG_ERROR("Failed to find suitable memory type");
    throw std::runtime_error("failed to find suitable memory type");
}
}