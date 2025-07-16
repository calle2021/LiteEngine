#include "VulkanContext.h"
#include "pch.h"

VulkanContext::VulkanContext()
    : m_Instance(nullptr)
    , m_Device(*this) {}

VulkanContext::~VulkanContext()
{
}

void VulkanContext::Init(Window *window)
{
    m_Window = window;
    CreateInstance();
    m_Device.CreateSurface();
    m_Device.PickPhysicalDevice();
    m_Device.CreateLogicalDevice();
}

void VulkanContext::CreateInstance()
{
    constexpr vk::ApplicationInfo AppInfo { 
        .pApplicationName = "Sample",
        .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
        .pEngineName = "Engine",
        .engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
        .apiVersion = vk::ApiVersion14 
    };

    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Check if the required GLFW extensions are supported by the Vulkan implementation.
    auto extensionProperties = m_Context.enumerateInstanceExtensionProperties();
    for (uint32_t i = 0; i < glfwExtensionCount; ++i)
    {
        if (std::ranges::none_of(extensionProperties,
                                [glfwExtension = glfwExtensions[i]](auto const& extensionProperty)
                                { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
        {
            throw std::runtime_error("Required GLFW extension not supported: " + std::string(glfwExtensions[i]));
        }
    }

    vk::InstanceCreateInfo CreateInfo {
        .pApplicationInfo = &AppInfo,
        .enabledExtensionCount = glfwExtensionCount,
        .ppEnabledExtensionNames = glfwExtensions,
    };
    m_Instance = vk::raii::Instance(m_Context, CreateInfo);
}