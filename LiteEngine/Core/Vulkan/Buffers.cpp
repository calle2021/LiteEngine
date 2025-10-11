#include "Buffers.h"
#include "Core/Logging/Logger.h"
#include "Config.h"
#include <unordered_map>

namespace LiteVulkan {
Buffers::Buffers(Device& dev, Renderer& rend, SwapChain& swap, Assets& assets, Camera& cam)
    : m_CameraRef(cam)
    , m_DeviceRef(dev)
    , m_RendererRef(rend)
    , m_SwapChainRef(swap)
    , m_AssetsRef(assets) {}

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
    UniformBufferObject ubo{};
    ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = m_CameraRef.GetView();
    ubo.proj = m_CameraRef.GetProjection();
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

void Buffers::BufferModels()
{
    std::unordered_map<Vertex, uint32_t> unique_verticies;

    for (const auto& shape : m_AssetsRef.shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex{};

            vertex.m_Pos = {
                m_AssetsRef.attrib.vertices[3 * index.vertex_index + 0],
                m_AssetsRef.attrib.vertices[3 * index.vertex_index + 1],
                m_AssetsRef.attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.m_Tex = {
                m_AssetsRef.attrib.texcoords[2 * index.texcoord_index + 0],
                // obj format defines 0 as bottom but in vulkan 0 is top
                // so we flip the vertical component
                1.0 - m_AssetsRef.attrib.texcoords[2 * index.texcoord_index + 1]
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