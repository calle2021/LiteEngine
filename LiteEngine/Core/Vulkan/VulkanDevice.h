#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <optional>
#include <cstdint>

class VulkanSwapChain;
class VulkanGraphicsPipeline;
class VulkanRenderer;
class VulkanContext;

class VulkanDevice
{
friend class VulkanSwapChain;
friend class VulkanGraphicsPipeline;
friend class VulkanRenderer;
friend class VulkanContext;
public:
    void PickPhysicalDevice(vk::raii::Instance* instance);
    void CreateLogicalDevice(vk::raii::SurfaceKHR* surface);
    void CreateSurface();
private:
    vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
    vk::raii::Device m_Device = nullptr;
    vk::raii::Queue m_Queue = nullptr;
    std::optional<uint32_t> m_QueueIndex;
};