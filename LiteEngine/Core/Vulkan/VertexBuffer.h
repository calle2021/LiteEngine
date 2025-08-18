#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "Device.h"
#include "Renderer.h"
#include <array>

namespace LiteVulkan {
class Renderer;
class VertexBuffer
{
friend class Renderer;
public:
    VertexBuffer(Device& device, Renderer&);
    void CreateVertexBuffer();
    void CreateIndexBuffer();
public:
    struct Vertex
    {
        glm::vec2 m_Pos;
        glm::vec3 m_Color;
        static vk::VertexInputBindingDescription GetBindingDescription() {
            return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
        }

        static std::array<vk::VertexInputAttributeDescription, 2> GetAttributeDescriptions() {
            return {
                vk::VertexInputAttributeDescription( 0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, m_Pos) ),
                vk::VertexInputAttributeDescription( 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, m_Color) )
            };
        }
    };
private:
    const std::vector<uint16_t> m_Indices = {0, 1, 2, 2, 3, 0};
private:
    void CreateBuffer(
        vk::DeviceSize size,
        vk::BufferUsageFlags usage,
        vk::MemoryPropertyFlags properties,
        vk::raii::Buffer& buffer, vk::raii::DeviceMemory& bufferMemory
    );
    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
    void CopyBuffer(vk::raii::Buffer& src, vk::raii::Buffer& dst, vk::DeviceSize size);
private:
    vk::raii::Buffer m_VertexBuffer = nullptr;
    vk::raii::DeviceMemory m_VertexBufferMemory = nullptr;
    vk::raii::Buffer m_IndexBuffer = nullptr;
    vk::raii::DeviceMemory m_IndexBufferMemory = nullptr;
private: // Class references
    Device& m_Device;
    Renderer& m_Renderer;
};
}