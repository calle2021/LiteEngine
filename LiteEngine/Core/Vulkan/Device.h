#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <optional>
#include <cstdint>

namespace LiteVulkan {
class SwapChain;
class Pipeline;
class Renderer;
class Context;
class Buffers;
class Texture;
class Device
{
friend class SwapChain;
friend class Pipeline;
friend class Renderer;
friend class Context;
friend class Buffers;
friend class Texture;
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