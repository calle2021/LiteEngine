#pragma once
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "Device.h"
#include <array>

namespace LiteVulkan {
class Renderer;
class VertexBuffer
{
friend class Renderer;
public:
    VertexBuffer(Device& device);
    void CreateVertexBuffer();
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
    uint32_t FindMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
private:
    vk::raii::Buffer m_VertexBuffer = nullptr;
    vk::raii::DeviceMemory m_VertexBufferMemory = nullptr;
private: // Class references
    Device& m_Device;
};
}