#include "vulkan-debug.h"
#include "pch.h"
#include <GLFW/glfw3.h>

namespace GameSystem
{
#ifdef NDEBUG
const bool use_validation_layers = false;
#else
const bool use_validation_layers = true;
#endif

const std::vector<const char *> validation_layers = {
    "VK_LAYER_KHRONOS_validation",
};

VkDebugUtilsMessengerEXT VulkanDebug::debug_messenger;

std::vector<const char *> VulkanDebug::GetRequiredExtenstions()
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

bool VulkanDebug::CheckValidationLayers()
{
    if (!use_validation_layers)
        return true;
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

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
                                                    VkDebugUtilsMessageTypeFlagsEXT type,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
                                                    void *pUserData)
{
    std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
    return VK_FALSE;
}

void VulkanDebug::SetupDebugMessenger()
{
    if (!use_validation_layers)
        return;
    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = DebugCallback;
    create_info.pUserData = nullptr;
}

} // namespace GameSystem