#include "VulkanDevice.h"
#include "VulkanContext.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

std::vector<const char*> DeviceExtensions = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

VulkanDevice::VulkanDevice(VulkanContext& context)
    : r_Context(context)
    , m_PhysicalDevice(nullptr)
    , m_Device(nullptr)
    , m_GraphicsQueue(nullptr)
    , m_Surface(nullptr) {}

VulkanDevice::~VulkanDevice() {}

void VulkanDevice::PickPhysicalDevice()
{
}

void VulkanDevice::CreateLogicalDevice() 
{
}

void VulkanDevice::CreateSurface()
{
}