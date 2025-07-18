#pragma once
#include <vulkan/vulkan_raii.hpp>

class VulkanContext;

class VulkanDevice
{
public:
    VulkanDevice(VulkanContext& context);
    ~VulkanDevice();
public:
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSurface();
private:
    vk::raii::PhysicalDevice m_PhysicalDevice;
    vk::raii::Device m_Device;
    vk::raii::Queue m_GraphicsQueue;
    vk::raii::SurfaceKHR m_Surface;
private:
    VulkanContext& r_Context;
};