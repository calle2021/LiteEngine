#include "VulkanDevice.h"
#include "VulkanContext.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Core/Logging/Logger.h"

std::vector<const char*> DeviceExtensions = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

void VulkanDevice::PickPhysicalDevice(vk::raii::Instance* instance)
{
    auto devices = instance->enumeratePhysicalDevices();
    CORE_LOG_INFO("Found {} potential physical devices.", devices.size());
    switch (devices.size())
    {
    case 0:
        CORE_LOG_ERROR("Failed to find any physical devices.");
        throw std::runtime_error("Failed to find any physical devices.");
        break;
    case 1:
        m_PhysicalDevice = devices[0];
        CORE_LOG_INFO("Successfully picked a physical device.");
        break;
    default:
        CORE_LOG_WARN("Multiple physical devices found, need logic to pick suitable device.");
        m_PhysicalDevice = devices[0];
        break;
    }
}

void VulkanDevice::CreateLogicalDevice() 
{
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();
    for(auto const& qfp : queueFamilyProperties) {
    }
}

void VulkanDevice::CreateSurface()
{
}