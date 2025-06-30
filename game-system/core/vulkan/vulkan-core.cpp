#include "vulkan-core.h"
#include "GraphicsPipeline.h"
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
VkSwapchainKHR VulkanCore::vk_swap_chain;

std::vector<VkImage> VulkanCore::swapChainImages;
std::vector<VkImageView> VulkanCore::swapChainImageViews;

VkFormat VulkanCore::swapChainImageFormat;
VkExtent2D VulkanCore::swapChainExtent;

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

    QueueFamilyIndices queue_familiy_indicies = VulkanUtils::FindQueueFamilies(vk_physical_device, vk_surface);

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::set<uint32_t> unique_queue_families = {queue_familiy_indicies.graphics_family.value(),
                                                queue_familiy_indicies.present_family.value()};

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
    q_create_info.queueFamilyIndex = queue_familiy_indicies.graphics_family.value();
    q_create_info.queueCount = 1;
    float q_prio = 1.0f;
    q_create_info.pQueuePriorities = &q_prio;

    VkPhysicalDeviceFeatures d_features = {};
    VkDeviceCreateInfo d_create_info = {};
    d_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    d_create_info.pQueueCreateInfos = &q_create_info;
    d_create_info.queueCreateInfoCount = 1;
    d_create_info.pEnabledFeatures = &d_features;
    d_create_info.enabledExtensionCount = static_cast<uint32_t>(VulkanUtils::device_extenstions.size());
    d_create_info.ppEnabledExtensionNames = VulkanUtils::device_extenstions.data();
    d_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
    d_create_info.pQueueCreateInfos = queue_create_infos.data();

    if (vkCreateDevice(vk_physical_device, &d_create_info, nullptr, &vk_device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vk device");
    }

    vkGetDeviceQueue(vk_device, queue_familiy_indicies.graphics_family.value(), 0, &vk_graphics_queue);
    vkGetDeviceQueue(vk_device, queue_familiy_indicies.present_family.value(), 0, &vk_present_queue);

    SwapChainSupportDetails swapChainSupport = VulkanUtils::QuerySwapChainSupport(vk_physical_device, vk_surface);
    VkSurfaceFormatKHR surfaceFormat = VulkanUtils::ChooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = VulkanUtils::ChooseSwapPresentMode(swapChainSupport.present_modes);
    VkExtent2D extent = VulkanUtils::ChooseSwapExtent(swapChainSupport.capabilities);
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swap_chain_create_info = {};
    swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_chain_create_info.surface = vk_surface;
    swap_chain_create_info.minImageCount = imageCount;
    swap_chain_create_info.imageFormat = surfaceFormat.format;
    swap_chain_create_info.imageColorSpace = surfaceFormat.colorSpace;
    swap_chain_create_info.imageExtent = extent;
    swap_chain_create_info.imageArrayLayers = 1;
    swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queue_familiy_indicies_arr[] = {queue_familiy_indicies.graphics_family.value(),
                                             queue_familiy_indicies.present_family.value()};
    if (queue_familiy_indicies.graphics_family != queue_familiy_indicies.present_family)
    {
        swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swap_chain_create_info.queueFamilyIndexCount = 2;
        swap_chain_create_info.pQueueFamilyIndices = queue_familiy_indicies_arr;
    }
    else
    {
        swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swap_chain_create_info.queueFamilyIndexCount = 0;     // Optional
        swap_chain_create_info.pQueueFamilyIndices = nullptr; // Optional
    }
    swap_chain_create_info.preTransform = swapChainSupport.capabilities.currentTransform;
    swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swap_chain_create_info.presentMode = presentMode;
    swap_chain_create_info.clipped = VK_TRUE;
    swap_chain_create_info.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(vk_device, &swap_chain_create_info, nullptr, &vk_swap_chain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    vkGetSwapchainImagesKHR(vk_device, vk_swap_chain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(vk_device, vk_swap_chain, &imageCount, swapChainImages.data());
    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

    swapChainImageViews.resize(swapChainImages.size());
    for (size_t i = 0; i < swapChainImages.size(); i++)
    {
        VkImageViewCreateInfo image_view_create_info = {};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = swapChainImages[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = swapChainImageFormat;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        if (vkCreateImageView(vk_device, &image_view_create_info, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create image views!");
        }
    }
    GraphicsPipeline m_gpipe;
    m_gpipe.LoadShaders();
}

void VulkanCore::Destroy()
{
    VulkanUtils::Destroy();
    for (auto imageView : swapChainImageViews)
    {
        vkDestroyImageView(vk_device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(vk_device, vk_swap_chain, nullptr);
    vkDestroyDevice(vk_device, nullptr);
    vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
    vkDestroyInstance(vk_instance, nullptr);
}

} // namespace GameSystem