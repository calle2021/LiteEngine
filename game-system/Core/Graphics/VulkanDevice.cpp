#include "VulkanDevice.h"

#include "VulkanContext.h"
#include "pch.h"

VulkanDevice::VulkanDevice(VulkanContext *context) : p_Context(context){};
VulkanDevice::~VulkanDevice(){};

void VulkanDevice::PickPhysicalDevice()
{
    p_Context->m_PhysicalDevice = VK_NULL_HANDLE;
    uint32_t x_Device = 0;
    vkEnumeratePhysicalDevices(p_Context->m_Instance, &x_Device, nullptr);
    if (!x_Device)
    {
        throw std::runtime_error("Failed to find a physical device");
    }

    std::vector<VkPhysicalDevice> Devices(x_Device);
    vkEnumeratePhysicalDevices(p_Context->m_Instance, &x_Device, Devices.data());

    for (const auto &Device : Devices)
    {
        bool res = IsDeviceSuitable(Device);
        if (res)
        {
            p_Context->m_PhysicalDevice = Device;
            break;
        }
    }

    if (p_Context->m_PhysicalDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a suitable device");
    }
}

void VulkanDevice::CreateLogicalDevice()
{
    m_QueueIndicies = FindQueueFamilies(p_Context->m_PhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
    std::set<uint32_t> QFamilies = {m_QueueIndicies.m_GraphicsFamily.value(), m_QueueIndicies.m_PresentFamily.value()};

    float QPriority = 1.0f;
    for (uint32_t QFamily : QFamilies)
    {
        VkDeviceQueueCreateInfo xQueueCreateInfo{};
        xQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        xQueueCreateInfo.queueFamilyIndex = QFamily;
        xQueueCreateInfo.queueCount = 1;
        xQueueCreateInfo.pQueuePriorities = &QPriority;
        QueueCreateInfos.push_back(xQueueCreateInfo);
    }

    VkPhysicalDeviceFeatures DeviceFeatures{};
    VkDeviceCreateInfo DeviceCreateInfo{};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.queueCreateInfoCount = 1;
    DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;
    DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
    DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
    DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCreateInfos.size());
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();

    if (vkCreateDevice(p_Context->m_PhysicalDevice, &DeviceCreateInfo, nullptr, &p_Context->m_Device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create device");
    }

    vkGetDeviceQueue(p_Context->m_Device, m_QueueIndicies.m_GraphicsFamily.value(), 0, &p_Context->m_GraphicsQueue);
    vkGetDeviceQueue(p_Context->m_Device, m_QueueIndicies.m_PresentFamily.value(), 0, &p_Context->m_PresentQueue);
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices QueueIndicies = FindQueueFamilies(device);
    bool extensions_supported = CheckDeviceExtensions(device);
    bool swap_chain_adequate = false;
    if (extensions_supported)
    {
        SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device);
        swap_chain_adequate = !swap_chain_support.m_Formats.empty() && !swap_chain_support.m_PresentModes.empty();
    }
    return QueueIndicies.IsComplete() && extensions_supported && swap_chain_adequate;
}

QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices QueueIndicies;

    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, nullptr);

    std::vector<VkQueueFamilyProperties> QueueFamilies(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &family_count, QueueFamilies.data());

    int i = 0;
    for (const auto &QFamily : QueueFamilies)
    {
        if (QFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            QueueIndicies.m_GraphicsFamily = i;
        }
        VkBool32 surface_supported = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, p_Context->m_Surface, &surface_supported);
        if (surface_supported)
        {
            QueueIndicies.m_PresentFamily = i;
        }
        if (QueueIndicies.IsComplete())
        {
            break;
        }
        i++;
    }

    return QueueIndicies;
}

bool VulkanDevice::CheckDeviceExtensions(VkPhysicalDevice device)
{
    uint32_t xExtensions;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &xExtensions, nullptr);

    std::vector<VkExtensionProperties> Extensions(xExtensions);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &xExtensions, Extensions.data());

    std::set<std::string> RequiredExtensions;
    for (const auto &ex : Extensions)
    {
        RequiredExtensions.insert(ex.extensionName);
    }

    for (const auto &extensions : Extensions)
    {
        RequiredExtensions.erase(extensions.extensionName);
    }

    return RequiredExtensions.empty();
}

SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(VkPhysicalDevice device)
{
    SwapChainSupportDetails SwapChainDetails;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, p_Context->m_Surface, &SwapChainDetails.m_Capabilities);
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, p_Context->m_Surface, &format_count, nullptr);

    if (format_count != 0)
    {
        SwapChainDetails.m_Formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, p_Context->m_Surface, &format_count,
                                             SwapChainDetails.m_Formats.data());
    }

    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, p_Context->m_Surface, &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
        SwapChainDetails.m_PresentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, p_Context->m_Surface, &present_mode_count,
                                                  SwapChainDetails.m_PresentModes.data());
    }
    return SwapChainDetails;
}
