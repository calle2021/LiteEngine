#pragma once
#include <vulkan/vulkan_raii.hpp>

class VulkanContext;

class VulkanDevice
{
public:
    void PickPhysicalDevice(vk::raii::Instance* instance);
    void CreateLogicalDevice();
    void CreateSurface();
private:
    vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
};