#include "VulkanBase.h"
#include <fstream>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
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

std::vector<char> VulkanBase::ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file");
    }

    std::vector<char> buffer(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
    file.close();
    std::cout << "Loaded file " << filename << std::endl;
    return buffer;
}