#include "vulkan-utils.h"
#include "pch.h"
#include <GLFW/glfw3.h>

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

bool VulkanUtils::IsDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    return indices.IsComplete();
}

QueueFamilyIndices VulkanUtils::FindQueueFamilies(VkPhysicalDevice device)
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
        if (indices.IsComplete())
        {
            break;
        }
        i++;
    }

    return indices;
}

void VulkanUtils::Destroy()
{
    if (!use_validation_layers)
    {
        return;
    }
}

} // namespace GameSystem