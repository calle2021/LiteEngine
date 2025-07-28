#pragma once
#include <vulkan/vulkan_raii.hpp>

class VulkanDevice
{
public:
    void PickPhysicalDevice(vk::raii::Instance* instance);
    void CreateLogicalDevice(vk::raii::SurfaceKHR* surface);
    void CreateSurface();
    vk::raii::PhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; };
    vk::raii::Device& GetDevice() { return m_Device; };
private:
    vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
    vk::raii::Device m_Device = nullptr;
    vk::raii::Queue m_GraphicsQueue = nullptr;
    vk::raii::Queue m_PresentQueue = nullptr;
};