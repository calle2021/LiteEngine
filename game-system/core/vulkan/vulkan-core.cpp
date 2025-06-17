#include "vulkan-core.h"
#include "pch.h"
#include "vulkan-debug.h"

namespace GameSystem
{
VkInstance VulkanCore::vk_instance;

void VulkanCore::Init()
{
    VulkanDebug::CheckValidationLayers();
    VkApplicationInfo vk_info = {};
    vk_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_info.pApplicationName = "Vulkan";
    vk_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_info.pEngineName = "Engine";
    vk_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_info.apiVersion = VK_API_VERSION_1_0;

    auto extensions = VulkanDebug::GetRequiredExtenstions();
    VkInstanceCreateInfo vk_create_info = {};
    vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_create_info.pApplicationInfo = &vk_info;
    vk_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    vk_create_info.ppEnabledExtensionNames = extensions.data();
    vk_create_info.enabledLayerCount = 0;

    if (vkCreateInstance(&vk_create_info, nullptr, &vk_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("vkCreateInstance failed");
    }
}

void VulkanCore::Destroy()
{
    vkDestroyInstance(vk_instance, nullptr);
}

} // namespace GameSystem