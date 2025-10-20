#pragma once
#include "Assets.h"
#include "Device.h"
#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_raii.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "Core/Camera/Camera.h"
#include <glm/gtx/hash.hpp>

using namespace LiteEngine;

namespace LiteVulkan {
class Renderer;
class Buffers
{
    friend class Renderer;

public:
    Buffers(const Device& device, const LiteEngine::Camera& camera);
    void BufferModels(const std::vector<tinyobj::shape_t>& shapes, const tinyobj::attrib_t& attrib);
    void CreateVertexBuffer(const vk::raii::CommandPool& command_pool);
    void CreateIndexBuffer(const vk::raii::CommandPool& command_pool);
    void CreateUniformBuffers(const uint32_t nFramesInFlight);
    void UpdateUniformBuffer(uint32_t curr);
    const std::vector<vk::raii::Buffer>& GetUniformBuffers() const { return m_UniformBuffers; };

public:
    static void CreateBuffer(const Device& device, vk::DeviceSize size, vk::BufferUsageFlags usage,
                             vk::MemoryPropertyFlags properties, vk::raii::Buffer& buffer,
                             vk::raii::DeviceMemory& bufferMemory);
    static uint32_t FindMemoryType(vk::PhysicalDeviceMemoryProperties memProperties,
                                   uint32_t typeFilter, vk::MemoryPropertyFlags properties);

public:
    struct Vertex
    {
        glm::vec3 m_Pos;
        glm::vec3 m_Color;
        glm::vec2 m_Tex;

        static vk::VertexInputBindingDescription GetBindingDescription()
        {
            return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
        }

        static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions()
        {
            return { vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat,
                                                         offsetof(Vertex, m_Pos)),
                     vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat,
                                                         offsetof(Vertex, m_Color)),
                     vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat,
                                                         offsetof(Vertex, m_Tex)) };
        }

        bool operator==(const Vertex& v) const
        {
            return m_Pos == v.m_Pos && m_Color == v.m_Color && m_Tex == v.m_Tex;
        }
    };

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
    const Camera& m_CameraRef;
};
}

namespace std {
template <> struct hash<LiteVulkan::Buffers::Vertex>
{
    size_t operator()(LiteVulkan::Buffers::Vertex const& vertex) const
    {
        return ((hash<glm::vec3>()(vertex.m_Pos) ^ (hash<glm::vec3>()(vertex.m_Color) << 1)) >> 1)
            ^ (hash<glm::vec2>()(vertex.m_Tex) << 1);
    }
};
}