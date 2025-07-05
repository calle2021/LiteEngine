#include "VulkanSwapChain.h"

#include <stdexcept>

#include "VulkanContext.h"
#include "pch.h"

VulkanSwapChain::VulkanSwapChain(VulkanContext *context) : p_Context(context)
{
}
VulkanSwapChain::~VulkanSwapChain()
{
}

void VulkanSwapChain::CreateSwapchain()
{
    m_SwapchainSupport = QuerySupport();
    m_SurfaceFormat = ChooseSurfaceFormat(m_SwapchainSupport.m_Formats);
    m_PresentMode = ChoosePresentMode(m_SwapchainSupport.m_PresentModes);
    m_Extent = ChooseExtent(m_SwapchainSupport.m_Capabilities);

    m_ImageCount = m_SwapchainSupport.m_Capabilities.minImageCount + 1;
    if (m_SwapchainSupport.m_Capabilities.minImageCount > 0 &&
        m_ImageCount > m_SwapchainSupport.m_Capabilities.minImageCount)
    {
        m_ImageCount = m_SwapchainSupport.m_Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapChainCreateInfo{};
    SwapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapChainCreateInfo.surface = p_Context->m_Surface;
    SwapChainCreateInfo.minImageCount = m_ImageCount;
    SwapChainCreateInfo.imageFormat = m_SurfaceFormat.format;
    SwapChainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
    SwapChainCreateInfo.imageExtent = m_Extent;
    SwapChainCreateInfo.imageArrayLayers = 1;
    SwapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    auto QFamilyIndicies = p_Context->c_DeviceManager.GetFamilyIndicies();
    uint32_t QIndiciesX[] = {QFamilyIndicies.m_GraphicsFamily.value(), QFamilyIndicies.m_PresentFamily.value()};

    if (QIndiciesX[0] != QIndiciesX[1])
    {
        SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SwapChainCreateInfo.queueFamilyIndexCount = 2;
        SwapChainCreateInfo.pQueueFamilyIndices = QIndiciesX;
    }
    else
    {
        SwapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        SwapChainCreateInfo.queueFamilyIndexCount = 0;     // Optional
        SwapChainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    }
    SwapChainCreateInfo.preTransform = m_SwapchainSupport.m_Capabilities.currentTransform;
    SwapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapChainCreateInfo.presentMode = m_PresentMode;
    SwapChainCreateInfo.clipped = VK_TRUE;
    SwapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(p_Context->m_Device, &SwapChainCreateInfo, nullptr, &p_Context->m_SwapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain");
    }
}

void VulkanSwapChain::CreateImageViews()
{
    vkGetSwapchainImagesKHR(p_Context->m_Device, p_Context->m_SwapChain, &m_ImageCount, nullptr);
    m_Images.resize(m_ImageCount);
    vkGetSwapchainImagesKHR(p_Context->m_Device, p_Context->m_SwapChain, &m_ImageCount, m_Images.data());
    m_ImageFormat = m_SurfaceFormat.format;

    m_ImageViews.resize(m_Images.size());
    for (size_t i = 0; i < m_Images.size(); i++)
    {
        VkImageViewCreateInfo ImageViewCreateInfo{};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image = m_Images[i];
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.format = m_ImageFormat;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;
        if (vkCreateImageView(p_Context->m_Device, &ImageViewCreateInfo, nullptr, &m_ImageViews[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create image view");
        }
    }
}

void VulkanSwapChain::Destroy()
{
    for (auto ImageView : m_ImageViews)
    {
        vkDestroyImageView(p_Context->m_Device, ImageView, nullptr);
    }
}

SwapChainSupportDetails VulkanSwapChain::QuerySupport()
{
    SwapChainSupportDetails Details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p_Context->m_PhysicalDevice, p_Context->m_Surface,
                                              &Details.m_Capabilities);
    uint32_t format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(p_Context->m_PhysicalDevice, p_Context->m_Surface, &format_count, nullptr);

    if (format_count != 0)
    {
        Details.m_Formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(p_Context->m_PhysicalDevice, p_Context->m_Surface, &format_count,
                                             Details.m_Formats.data());
    }
    uint32_t present_mode_count;
    vkGetPhysicalDeviceSurfacePresentModesKHR(p_Context->m_PhysicalDevice, p_Context->m_Surface, &present_mode_count,
                                              nullptr);

    if (present_mode_count != 0)
    {
        Details.m_PresentModes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(p_Context->m_PhysicalDevice, p_Context->m_Surface,
                                                  &present_mode_count, Details.m_PresentModes.data());
    }
    return Details;
}

VkSurfaceFormatKHR VulkanSwapChain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats)
{
    for (const auto &format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }
    return formats[0];
}

VkPresentModeKHR VulkanSwapChain::ChoosePresentMode(const std::vector<VkPresentModeKHR> &present_modes)
{
    for (const auto &present_mode : present_modes)
    {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::ChooseExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(p_Context->p_Window->GetWindowHandle(), &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}