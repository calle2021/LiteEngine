#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <optional>
#include <cstdint>

namespace LiteVulkan {
class SwapChain;
class GraphicsPipeline;
class Renderer;
class Context;
class VertexBuffer;
class Device
{
friend class SwapChain;
friend class GraphicsPipeline;
friend class Renderer;
friend class Context;
friend class VertexBuffer;
public:
    void PickPhysicalDevice(vk::raii::Instance& instance);
    void CreateLogicalDevice(vk::raii::SurfaceKHR& surface);
private:
    vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
    vk::raii::Device m_Device = nullptr;
    vk::raii::Queue m_Queue = nullptr;
    std::optional<uint32_t> m_QueueIndex;
};
}