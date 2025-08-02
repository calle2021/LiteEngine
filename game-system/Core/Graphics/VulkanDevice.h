#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <optional>
#include <cstdint>

class VulkanRenderer;

class VulkanDevice
{
friend class VulkanRenderer;
public:
    void PickPhysicalDevice(vk::raii::Instance* instance);
    void CreateLogicalDevice(vk::raii::SurfaceKHR* surface);
    void CreateSurface();
public:
    vk::raii::PhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; };
    vk::raii::Device& GetDevice() { return m_Device; };
    uint32_t GetGraphicsIndex() { return m_GraphicsIndex.value(); };
    vk::raii::Queue& GetGraphicsQueue() { return m_GraphicsQueue; };
    vk::raii::Queue& GetPresentQueue() { return m_PresentQueue; };
private:
    vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
    vk::raii::Device m_Device = nullptr;
    vk::raii::Queue m_GraphicsQueue = nullptr;
    vk::raii::Queue m_PresentQueue = nullptr;
    std::optional<uint32_t> m_GraphicsIndex;
    std::optional<uint32_t> m_PresentIndex;
};