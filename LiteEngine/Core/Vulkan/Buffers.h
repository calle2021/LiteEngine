#pragma once
#include "Device.h"
#include "Vertex.h"
#include "tiny_obj_loader.h"
#include <vulkan/vulkan_raii.hpp>

namespace LiteVulkan {
class Renderer;
class Buffers
{
    friend class Renderer;

public:
    Buffers(const Device& device);
    void BufferModels(const std::vector<tinyobj::shape_t>& shapes, const tinyobj::attrib_t& attrib);
    void CreateVertexBuffer(const vk::raii::CommandPool& command_pool);
    void CreateIndexBuffer(const vk::raii::CommandPool& command_pool);
    void CreateUniformBuffers(const uint32_t MAX_FRAMES_IN_FLIGHT);
    void UpdateUniformBuffer(const glm::mat4 view_matrix, const glm::mat4 proj_matrix,
                             uint32_t curr);
    const std::vector<vk::raii::Buffer>& GetUniformBuffers() const { return m_UniformBuffers; };

public:
    static void CreateBuffer(const Device& device, vk::DeviceSize size, vk::BufferUsageFlags usage,
                             vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer,
                             vk::raii::DeviceMemory& buffer_memory);
    static uint32_t FindMemoryType(vk::PhysicalDeviceMemoryProperties memory_props,
                                   uint32_t type_filter, vk::MemoryPropertyFlags props);

public:
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

private:
    std::vector<Vertex> m_Verticies;
    std::vector<uint32_t> m_Indices;

private:
    void CopyBuffer(const vk::raii::CommandPool& command_pool, vk::raii::Buffer& src,
                    vk::raii::Buffer& dst, vk::DeviceSize size);

private:
    vk::raii::Buffer m_Buffers = nullptr;
    vk::raii::DeviceMemory m_BuffersMemory = nullptr;
    vk::raii::Buffer m_IndexBuffer = nullptr;
    vk::raii::DeviceMemory m_IndexBufferMemory = nullptr;
    std::vector<vk::raii::Buffer> m_UniformBuffers;
    std::vector<vk::raii::DeviceMemory> m_UniformBuffersMemory;
    std::vector<void*> m_UniformBuffersMapped;
    vk::raii::Image m_Texture = nullptr;
    vk::raii::DeviceMemory m_TextureMemory = nullptr;

private: // Class references
    const Device& m_DeviceRef;
};
}