#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <vulkan/vulkan_raii.hpp>
#include "Device.h"
#include "Renderer.h"
#include "SwapChain.h"
#include <array>

namespace LiteVulkan {
class Renderer;
class Texture;
class Pipeline;
class Buffers
{
friend class Renderer;
friend class Texture;
friend class Pipeline;
public:
    Buffers(Device& dev, Renderer& rend, SwapChain& swap);
    void CreateVertexBuffer();
    void CreateIndexBuffer();
    void CreateUniformBuffers();
    void UpdateUniformBuffer(uint32_t curr);
public:
    struct Vertex
    {
        glm::vec3 m_Pos;
        glm::vec3 m_Color;
        glm::vec2 m_Tex;
        static vk::VertexInputBindingDescription GetBindingDescription() {
            return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
        }

        static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions() {
            return {
                vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, m_Pos) ),
                vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, m_Color)),
                vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, m_Tex))
            };
        }
    };

    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };

private:
    const std::vector<uint16_t> m_Indices =
    {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4
    };


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
    Device& m_DeviceRef;
    Renderer& m_RendererRef;
    SwapChain& m_SwapChainRef;
};
}