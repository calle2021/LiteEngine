#include "vulkan-core.h"
#include "pch.h"
#include <GLFW/glfw3.h>

namespace GameSystem
{
VkInstance VulkanCore::vk_instance;

#ifdef NDEBUG
const bool use_validation_layers = false;
#else
const bool use_validation_layers = true;
#endif

const std::vector<const char *> validation_layers = {
    "VK_LAYER_KHRONOS_validation",
};

void VulkanCore::Init()
{
    if (use_validation_layers)
    {
        if (!CheckValidationLayers())
        {
            throw std::runtime_error("CheckValidationLayers returned false");
        }
    }
    VkApplicationInfo vk_info = {};
    vk_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    vk_info.pApplicationName = "Vulkan";
    vk_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_info.pEngineName = "Engine";
    vk_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    vk_info.apiVersion = VK_API_VERSION_1_0;

    uint32_t glfw_extension_count = 0;
    const char **glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
    VkInstanceCreateInfo vk_create_info = {};
    vk_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    vk_create_info.pApplicationInfo = &vk_info;
    vk_create_info.enabledExtensionCount = glfw_extension_count;
    vk_create_info.ppEnabledExtensionNames = glfw_extensions;
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

bool VulkanCore::CheckValidationLayers()
{
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

} // namespace GameSystem