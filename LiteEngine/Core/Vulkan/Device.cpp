#include "Device.h"
#include "Context.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Core/Logging/Logger.h"

std::vector<const char*> RequiredDeviceExtension  = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};
namespace LiteVulkan {
void Device::PickPhysicalDevice(vk::raii::Instance& instance)
{
    auto devices = instance.enumeratePhysicalDevices();
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

void Device::CreateLogicalDevice(vk::raii::SurfaceKHR& surface)
{
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_PhysicalDevice.getQueueFamilyProperties();

    for(size_t i = 0; i < queueFamilyProperties.size(); i++) {
        bool graphics_support = (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlags{};
        bool preset_support = m_PhysicalDevice.getSurfaceSupportKHR(i , *surface );
        uint32_t idx =  static_cast<uint32_t>(i);
        if (graphics_support && preset_support) {
            m_QueueIndex = static_cast<uint32_t>(i);
            break;
        }
    }

    if (!m_QueueIndex.has_value()) {
        throw std::runtime_error("Failed to find queue with graphics and present support.");
    }
    CORE_LOG_INFO("Found queue family ({}).", m_QueueIndex.value());

    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> featureChain = {
        {.features = { .samplerAnisotropy = true }},                                                     // vk::PhysicalDeviceFeatures2
        {.synchronization2 = true, .dynamicRendering = true },  // vk::PhysicalDeviceVulkan13Features
        {.extendedDynamicState = true }                         // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
    };

    float queuePriority = 0.0f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{ .queueFamilyIndex = m_QueueIndex.value(), .queueCount = 1, .pQueuePriorities = &queuePriority };
    vk::DeviceCreateInfo deviceCreateInfo {
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(RequiredDeviceExtension.size()),
        .ppEnabledExtensionNames = RequiredDeviceExtension.data()
    };

    m_Device = vk::raii::Device(m_PhysicalDevice, deviceCreateInfo);
    CORE_LOG_INFO("Device created.");
    m_Queue = vk::raii::Queue(m_Device, m_QueueIndex.value(), 0);
    CORE_LOG_INFO("Queue created.");
}
}