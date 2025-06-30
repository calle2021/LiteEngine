#include "vulkan-utils.h"
#include "core/window/window.h"
#include "pch.h"
#include <GLFW/glfw3.h>
#include <fstream>

namespace GameSystem
{
#ifdef NDEBUG
const bool use_validation_layers = false;
#else
const bool use_validation_layers = false;
#endif

const std::vector<const char *> VulkanUtils::validation_layers = {
    "VK_LAYER_KHRONOS_validation",
};

const std::vector<const char *> VulkanUtils::device_extenstions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

std::vector<const char *> VulkanUtils::GetRequiredExtenstions()
{
    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector<const char *> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if (use_validation_layers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool VulkanUtils::CheckValidationLayers()
{
    if (!use_validation_layers)
    {
        return true;
    }
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> avaliable_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, avaliable_layers.data());

    for (const char *name : validation_layers)
    {
        bool layer_exsits = false;
        for (const auto &layer_props : avaliable_layers)
        {
            if (strcmp(name, layer_props.layerName) != 0)
                continue;
            layer_exsits = true;
            break;
        }

        if (!layer_exsits)
        {
            return false;
        }
    }

    return true;
}

bool VulkanUtils::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices = FindQueueFamilies(device, surface);
    bool extensions_supported = CheckDeviceExtensions(device);
    bool swap_chain_adequate = false;
    if (extensions_supported)
    {
        SwapChainSupportDetails swap_chain_support = QuerySwapChainSupport(device, surface);
        swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
    }
    return indices.IsComplete() && extensions_supported && swap_chain_adequate;
}

bool VulkanUtils::CheckDeviceExtensions(VkPhysicalDevice device)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> available_extension(extension_count);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extension.data());

    std::set<std::string> required_extensions(device_extenstions.begin(), device_extenstions.end());

    for (const auto &extension : available_extension)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

QueueFamilyIndices VulkanUtils::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    uint32_t q_family_x = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &q_family_x, nullptr);

    std::vector<VkQueueFamilyProperties> q_families(q_family_x);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &q_family_x, q_families.data());

    int i = 0;
    for (const auto &q_family : q_families)
    {
        if (q_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphics_family = i;
        }
        VkBool32 present_support = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &present_support);
        if (present_support)
        {
            indices.present_family = i;
        }
        if (indices.IsComplete())
        {
            break;
        }
        i++;
    }

    return indices;
}

SwapChainSupportDetails VulkanUtils::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, nullptr);

    if (format_count != 0)
    {
        details.formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &format_count, details.formats.data());
    }
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, nullptr);

    if (present_mode_count != 0)
    {
        details.present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &present_mode_count, details.present_modes.data());
    }
    return details;
}

// chose sRGB if avaliable
VkSurfaceFormatKHR VulkanUtils::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto &availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }
    return availableFormats[0];
}

VkPresentModeKHR VulkanUtils::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanUtils::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(Window::GetWindowHandle(), &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

std::vector<char> VulkanUtils::readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();
    return buffer;
}

void VulkanUtils::Destroy()
{
    if (!use_validation_layers)
    {
        return;
    }
}

} // namespace GameSystem