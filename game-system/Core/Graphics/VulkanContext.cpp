#include "VulkanContext.h"

#include "pch.h"

VulkanContext::VulkanContext() : c_SwapChain(this), c_Base(this), c_DeviceManager(this), m_GraphicsPipeline(this)
{
}

VulkanContext::~VulkanContext()
{
}

void VulkanContext::Init(Window *window)
{
    p_Window = window;
    CreateInstance();
    CreateSurface();
    c_DeviceManager.PickPhysicalDevice();
    c_DeviceManager.CreateLogicalDevice();
    c_SwapChain.CreateSwapchain();
    c_SwapChain.CreateImageViews();
    m_GraphicsPipeline.CreateRenderPass();
    m_GraphicsPipeline.CreateGraphicsPipeline();
}

void VulkanContext::CreateInstance()
{
    VkApplicationInfo AppInfo{};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.pApplicationName = "Vulkan";
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "Engine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_0;

    auto extensions = c_Base.GetRequiredExtenstions();
    VkInstanceCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    CreateInfo.pApplicationInfo = &AppInfo;
    CreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    CreateInfo.ppEnabledExtensionNames = extensions.data();
    CreateInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&CreateInfo, nullptr, &m_Instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create vk instance");
    }
}

void VulkanContext::CreateSurface()
{
    if (glfwCreateWindowSurface(m_Instance, p_Window->GetWindowHandle(), nullptr, &m_Surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create window surface");
    }
}

void VulkanContext::Destroy()
{
    m_GraphicsPipeline.Destroy();
    c_SwapChain.Destroy();
    vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
    vkDestroyDevice(m_Device, nullptr);
    vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
}