#include "vulkan-core.h"
#include "core/window/window.h"
#include "pch.h"
#include "vulkan-utils.h"

namespace GameSystem
{

VkInstance VulkanCore::vk_instance;
VkPhysicalDevice VulkanCore::vk_physical_device;
VkDevice VulkanCore::vk_device;
VkQueue VulkanCore::vk_graphics_queue;
VkSurfaceKHR VulkanCore::vk_surface;
VkQueue VulkanCore::vk_present_queue;

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

    if (glfwCreateWindowSurface(vk_instance, Window::GetWindowHandle(), nullptr, &vk_surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }

    vk_physical_device = VK_NULL_HANDLE;
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
    // and prefer physical devices that supports drawing and presentation
    // https://docs.vulkan.org/tutorial/latest/03_Drawing_a_triangle/01_Presentation/00_Window_surface.html
    for (const auto &device : devices)
    {
        if (VulkanUtils::IsDeviceSuitable(device, vk_surface))
        {
            vk_physical_device = device;
            break;
        }
    }

    if (vk_physical_device == VK_NULL_HANDLE)
    {
        throw std::runtime_error("Failed to find a vulkan suitable device");
    }

    QueueFamilyIndices physical_device_indicies = VulkanUtils::FindQueueFamilies(vk_physical_device, vk_surface);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {physical_device_indicies.graphics_family.value(),
                                                physical_device_indicies.present_family.value()};

    float queuePriority = 1.0f;
    for (uint32_t queue_family : unique_queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info = {};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queuePriority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkDeviceQueueCreateInfo q_create_info = {};
    q_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    q_create_info.queueFamilyIndex = physical_device_indicies.graphics_family.value();
    q_create_info.queueCount = 1;
    float q_prio = 1.0f;
    q_create_info.pQueuePriorities = &q_prio;

    VkPhysicalDeviceFeatures d_features = {};
    VkDeviceCreateInfo d_create_info = {};
    d_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    d_create_info.pQueueCreateInfos = &q_create_info;
    d_create_info.queueCreateInfoCount = 1;
    d_create_info.pEnabledFeatures = &d_features;
    d_create_info.ppEnabledExtensionNames = VulkanUtils::device_extenstions.data();
    d_create_info.enabledExtensionCount = static_cast<uint32_t>(VulkanUtils::device_extenstions.size());
    d_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    d_create_info.pQueueCreateInfos = queue_create_infos.data();

    if (vkCreateDevice(vk_physical_device, &d_create_info, nullptr, &vk_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vk device");
    }

    vkGetDeviceQueue(vk_device, physical_device_indicies.graphics_family.value(), 0, &vk_graphics_queue);
    vkGetDeviceQueue(vk_device, physical_device_indicies.present_family.value(), 0, &vk_present_queue);
}

void VulkanCore::Destroy()
{
    VulkanUtils::Destroy();
    vkDestroyDevice(vk_device, nullptr);
    vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
    vkDestroyInstance(vk_instance, nullptr);
}

} // namespace GameSystem