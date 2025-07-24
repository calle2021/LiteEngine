#include "VulkanDevice.h"
#include "VulkanContext.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Core/Logging/Logger.h"

std::vector<const char*> RequiredDeviceExtension  = {
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
    std::optional<uint32_t> graphicsIndex;
    for(size_t i = 0; i < queueFamilyProperties.size(); i++) {
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            graphicsIndex = static_cast<uint32_t>(i);
        }
    }
    if (graphicsIndex.has_value()) {
        CORE_LOG_INFO("Found queue family with graphics support.");
    } else {
        throw std::runtime_error("Failed to find queue family with graphics support.");
    }

    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = 
    {
        {},
        {.dynamicRendering = true },
        {.extendedDynamicState = true }
    };

    float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{ .queueFamilyIndex = graphicsIndex.value(), .queueCount = 1, .pQueuePriorities = &queuePriority };
    vk::DeviceCreateInfo deviceCreateInfo { 
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(RequiredDeviceExtension.size()),
        .ppEnabledExtensionNames = RequiredDeviceExtension.data()
    };

    m_Device = vk::raii::Device(m_PhysicalDevice, deviceCreateInfo);
    CORE_LOG_INFO("Device created.");
    m_GraphicsQueue = vk::raii::Queue(m_Device, graphicsIndex.value(), 0);
    CORE_LOG_INFO("Graphics queue created.");

}

void VulkanDevice::CreateSurface()
{
}