#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <vulkan/vulkan_raii.hpp>
#include "Device.h"
#include "Renderer.h"
#include "SwapChain.h"
#include "Assets.h"
#include <array>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "Core/Camera/Camera.h"

using namespace LiteEngine;

namespace LiteVulkan {
class Renderer;
class Assets;
class Pipeline;
class Buffers
{
friend class Renderer;
friend class Assets;
friend class Pipeline;
public:
    Buffers(Device& dev, Renderer& rend, SwapChain& swap, Assets& assets, Camera& cam);
    void BufferModels();
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
                vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, m_Pos)),
                vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, m_Color)),
                vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, m_Tex))
            };
        }

        bool operator==(const Vertex& v) const {
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
private:
    Camera& m_CameraRef;
private: // Class references
    Device& m_DeviceRef;
    Renderer& m_RendererRef;
    SwapChain& m_SwapChainRef;
    Assets& m_AssetsRef;
};
}

namespace std {
    template<> struct hash<LiteVulkan::Buffers::Vertex> {
        size_t operator()(LiteVulkan::Buffers::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.m_Pos) ^
                   (hash<glm::vec3>()(vertex.m_Color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.m_Tex) << 1);
        }
    };
}