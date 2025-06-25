#include "vulkan-core.h"
#include "pch.h"
#include "vulkan-utils.h"

namespace GameSystem
{

VkInstance VulkanCore::vk_instance;
VkPhysicalDevice VulkanCore::physical_device;
VkDevice VulkanCore::vk_device;
VkQueue VulkanCore::vk_graphics_q;

void VulkanCore::Init()
{
    VulkanUtils::CheckValidationLayers();

    VkApplicationInfo vk_info = {};
    vk_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_info.pApplicationName = "Vulkan";
    vk_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_info.pEngineName = "Engine";
    vk_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_info.apiVersion = VK_API_VERSION_1_0;

    auto extensions = VulkanUtils::GetRequiredExtenstions();
    VkInstanceCreateInfo vk_create_info = {};
    vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_create_info.pApplicationInfo = &vk_info;
    vk_create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    vk_create_info.ppEnabledExtensionNames = extensions.data();
    vk_create_info.enabledLayerCount = 0;

    if (vkCreateInstance(&vk_create_info, nullptr, &vk_instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vk instance");
    }

    physical_device = VK_NULL_HANDLE;
    uint32_t device_x = 0;
    vkEnumeratePhysicalDevices(vk_instance, &device_x, nullptr);
    if (!device_x)
    {
        throw std::runtime_error("Failed to find a physical device");
    }

    std::vector<VkPhysicalDevice> devices(device_x);
    vkEnumeratePhysicalDevices(vk_instance, &device_x, devices.data());

    // TODO: extend to pick the best device
    // https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/00_Setup/03_Physical_devices_and_queue_families.html
    for (const auto &d : devices)
    {
        if (VulkanUtils::IsDeviceSuitable(d))
        {
            physical_device = d;
            break;
        }
    }

    if (physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a vulkan suitable device");
    }

    QueueFamilyIndices pd_indices = VulkanUtils::FindQueueFamilies(physical_device);

    VkDeviceQueueCreateInfo q_create_info = {};
    q_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    q_create_info.queueFamilyIndex = pd_indices.graphics_family.value();
    q_create_info.queueCount = 1;
    float q_prio = 1.0f;
    q_create_info.pQueuePriorities = &q_prio;

    VkPhysicalDeviceFeatures d_features = {};
    VkDeviceCreateInfo d_create_info = {};
    d_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    d_create_info.pQueueCreateInfos = &q_create_info;
    d_create_info.queueCreateInfoCount = 1;
    d_create_info.pEnabledFeatures = &d_features;
    d_create_info.enabledExtensionCount = 0;

    if (vkCreateDevice(physical_device, &d_create_info, nullptr, &vk_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vk device");
    }

    vkGetDeviceQueue(vk_device, pd_indices.graphics_family.value(), 0, &vk_graphics_q);
}

void VulkanCore::Destroy()
{
    VulkanUtils::Destroy();
    vkDestroyDevice(vk_device, nullptr);
    vkDestroyInstance(vk_instance, nullptr);
}

} // namespace GameSystem