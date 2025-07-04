#include "VulkanBase.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "pch.h"

#ifdef NDEBUG
const bool VALIDATIONLAYERS = false;
#else
const bool VALIDATIONLAYERS = false;
#endif

VulkanBase::VulkanBase(VulkanContext *context) : p_Context(context){};
VulkanBase::~VulkanBase(){};
std::vector<const char *> VulkanBase::GetRequiredExtenstions()
{
    uint32_t glfw_ExtensionCount = 0;
    const char **glfw_Extensions;
    glfw_Extensions = glfwGetRequiredInstanceExtensions(&glfw_ExtensionCount);

    std::vector<const char *> Extensions(glfw_Extensions, glfw_Extensions + glfw_ExtensionCount);

    if (VALIDATIONLAYERS)
    {
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return Extensions;
}