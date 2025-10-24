#include "Buffers.h"
#include "Core/Logging/Logger.h"
#include <unordered_map>

namespace LiteVulkan {
Buffers::Buffers(const Device& device)
    : m_DeviceRef(device)
{
}

void Buffers::CreateBuffer(const Device& device, vk::DeviceSize size, vk::BufferUsageFlags usage,
                           vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer,
                           vk::raii::DeviceMemory& buffer_memory)
{
    vk::BufferCreateInfo buffer_info = {};
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = vk::SharingMode::eExclusive;

    buffer = vk::raii::Buffer(device.GetDevice(), buffer_info);
    vk::MemoryRequirements mem_requirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo mem_alloc_info = {};
    mem_alloc_info.allocationSize = mem_requirements.size;
    mem_alloc_info.memoryTypeIndex
        = FindMemoryType(device.GetPhysicalDevice().getMemoryProperties(),
                         mem_requirements.memoryTypeBits, properties);

    buffer_memory = vk::raii::DeviceMemory(device.GetDevice(), mem_alloc_info);
    buffer.bindMemory(buffer_memory, 0);
}

void Buffers::CreateVertexBuffer(const vk::raii::CommandPool& command_pool)
{
    vk::DeviceSize buffer_sz = sizeof(m_Verticies[0]) * m_Verticies.size();
    vk::raii::Buffer staging_buffer({});
    vk::raii::DeviceMemory staging_buffer_memory({});
    CreateBuffer(m_DeviceRef, buffer_sz, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible
                     | vk::MemoryPropertyFlagBits::eHostCoherent,
                 staging_buffer, staging_buffer_memory);

    void* data = staging_buffer_memory.mapMemory(0, buffer_sz);
    memcpy(data, m_Verticies.data(), buffer_sz);
    staging_buffer_memory.unmapMemory();

    CreateBuffer(m_DeviceRef, buffer_sz,
                 vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, m_Buffers, m_BuffersMemory);

    CopyBuffer(command_pool, staging_buffer, m_Buffers, buffer_sz);
    CORE_LOG_INFO("Vertex buffer created");
}

void Buffers::CreateIndexBuffer(const vk::raii::CommandPool& command_pool)
{
    vk::DeviceSize buffer_sz = sizeof(m_Indices[0]) * m_Indices.size();

    vk::raii::Buffer staging_buffer({});
    vk::raii::DeviceMemory staging_buffer_memory({});
    CreateBuffer(m_DeviceRef, buffer_sz, vk::BufferUsageFlagBits::eTransferSrc,
                 vk::MemoryPropertyFlagBits::eHostVisible
                     | vk::MemoryPropertyFlagBits::eHostCoherent,
                 staging_buffer, staging_buffer_memory);

    void* data = staging_buffer_memory.mapMemory(0, buffer_sz);
    memcpy(data, m_Indices.data(), (size_t)buffer_sz);
    staging_buffer_memory.unmapMemory();

    CreateBuffer(m_DeviceRef, buffer_sz,
                 vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
                 vk::MemoryPropertyFlagBits::eDeviceLocal, m_IndexBuffer, m_IndexBufferMemory);

    CopyBuffer(command_pool, staging_buffer, m_IndexBuffer, buffer_sz);
    CORE_LOG_INFO("Index buffer created");
}

void Buffers::CreateUniformBuffers(const uint32_t MAX_FRAMES_IN_FLIGHT)
{
    m_UniformBuffers.clear();
    m_UniformBuffersMemory.clear();
    m_UniformBuffersMapped.clear();

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vk::DeviceSize buffer_sz = sizeof(UniformBufferObject);
        vk::raii::Buffer buffer({});
        vk::raii::DeviceMemory buffer_memory({});
        CreateBuffer(m_DeviceRef, buffer_sz, vk::BufferUsageFlagBits::eUniformBuffer,
                     vk::MemoryPropertyFlagBits::eHostVisible
                         | vk::MemoryPropertyFlagBits::eHostCoherent,
                     buffer, buffer_memory);
        m_UniformBuffers.emplace_back(std::move(buffer));
        m_UniformBuffersMemory.emplace_back(std::move(buffer_memory));
        m_UniformBuffersMapped.emplace_back(m_UniformBuffersMemory[i].mapMemory(0, buffer_sz));
    }
    CORE_LOG_INFO("Uniform buffers created");
}

void Buffers::UpdateUniformBuffer(const glm::mat4 view_matrix, const glm::mat4 proj_matrix,
                                  uint32_t curr)
{
    UniformBufferObject ubo {};
    ubo.model = glm::mat4(1.0f);
    ubo.view = view_matrix;
    ubo.proj = proj_matrix;
    ubo.proj[1][1] *= -1;
    memcpy(m_UniformBuffersMapped[curr], &ubo, sizeof(ubo));
}

void Buffers::CopyBuffer(const vk::raii::CommandPool& command_pool, vk::raii::Buffer& src,
                         vk::raii::Buffer& dst, vk::DeviceSize size)
{
    vk::CommandBufferAllocateInfo command_buffer_alloc = {};
    command_buffer_alloc.commandPool = command_pool;
    command_buffer_alloc.level = vk::CommandBufferLevel::ePrimary;
    command_buffer_alloc.commandBufferCount = 1;

    vk::raii::CommandBuffer command_copy_buffer
        = std::move(m_DeviceRef.GetDevice().allocateCommandBuffers(command_buffer_alloc).front());
    command_copy_buffer.begin(
        vk::CommandBufferBeginInfo { .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
    command_copy_buffer.copyBuffer(*src, *dst, vk::BufferCopy { .size = size });
    command_copy_buffer.end();
    m_DeviceRef.GetQueue().submit(
        vk::SubmitInfo { .commandBufferCount = 1, .pCommandBuffers = &*command_copy_buffer },
        nullptr);
    m_DeviceRef.GetQueue().waitIdle();
}

void Buffers::BufferModels(const std::vector<tinyobj::shape_t>& shapes,
                           const tinyobj::attrib_t& attrib)
{
    std::unordered_map<Vertex, uint32_t> unique_verticies;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex {};

            vertex.pos = { attrib.vertices[3 * index.vertex_index + 0],
                           attrib.vertices[3 * index.vertex_index + 1],
                           attrib.vertices[3 * index.vertex_index + 2] };

            vertex.tex_coord = { attrib.texcoords[2 * index.texcoord_index + 0],
                                 // obj format defines 0 as bottom but in vulkan 0 is top
                                 // so we flip the vertical component
                                 1.0 - attrib.texcoords[2 * index.texcoord_index + 1] };
            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (unique_verticies.count(vertex) == 0) {
                unique_verticies[vertex] = static_cast<uint32_t>(m_Verticies.size());
                m_Verticies.push_back(vertex);
            }

            m_Indices.push_back(unique_verticies[vertex]);
        }
    }
}

uint32_t Buffers::FindMemoryType(vk::PhysicalDeviceMemoryProperties memory_props,
                                 const uint32_t type_filter, vk::MemoryPropertyFlags props)
{
    for (uint32_t i = 0; i < memory_props.memoryTypeCount; i++) {
        if ((type_filter & (1 << i))
            && (memory_props.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}
}