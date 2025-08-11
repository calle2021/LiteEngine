#include "VulkanSwapChain.h"
#include "Core/Logging/Logger.h"

VulkanSwapChain::VulkanSwapChain(VulkanDevice& device, vk::raii::SurfaceKHR& surface, GLFWindow& window)
    : m_VulkanDevice(device)
    , m_Surface(surface)
    , m_Window(window) {}

void VulkanSwapChain::CreateSwapChain()
{
    auto surfaceCapabilities = m_VulkanDevice.m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);
    m_ImageFormat = ChooseSwapchainSurfaceFormat(m_VulkanDevice.m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface));
    m_Extent = ChooseSwapExtent(surfaceCapabilities);
    auto minImageCount = std::max( 3u, surfaceCapabilities.minImageCount );
    minImageCount = ( surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount ) ? surfaceCapabilities.maxImageCount : minImageCount;
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = m_Surface,
        .minImageCount = minImageCount,
        .imageFormat = m_ImageFormat,
        .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = m_Extent,
        .imageArrayLayers =1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = ChooseSwapPresentMode(m_VulkanDevice.m_PhysicalDevice.getSurfacePresentModesKHR(m_Surface)),
        .clipped = true };

    m_SwapChain = vk::raii::SwapchainKHR(m_VulkanDevice.m_Device, swapChainCreateInfo);
    CORE_LOG_INFO("Swap chain created");
    m_Images = m_SwapChain.getImages();
    CORE_LOG_INFO("Using {} images", m_Images.size());
}

void VulkanSwapChain::CreateImageViews()
{
    m_ImageViews.clear();

    vk::ImageViewCreateInfo imageViewCreateInfo {
        .viewType = vk::ImageViewType::e2D,
        .format = m_ImageFormat,
        .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
    };
    CORE_LOG_INFO("Found {} image(s)", m_Images.size());
    for ( auto image : m_Images )
    {
        imageViewCreateInfo.image = image;
        m_ImageViews.emplace_back(m_VulkanDevice.m_Device, imageViewCreateInfo);
    }
    CORE_LOG_INFO("Image views created");
}

void VulkanSwapChain::RecreateSwapChain()
{
    CORE_LOG_INFO("Recreating swap chain");

    int width = 0, height = 0;
    glfwGetFramebufferSize(m_Window.GetWindowHandle(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_Window.GetWindowHandle(), &width, &height);
        glfwWaitEvents();
    }

    m_VulkanDevice.m_Device.waitIdle();
    m_ImageViews.clear();
    m_SwapChain = nullptr;
    CreateSwapChain();
    CreateImageViews();
}

vk::Format VulkanSwapChain::ChooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    CORE_LOG_INFO("Found {} surface format(s)", availableFormats.size());
    for(const auto& format : availableFormats)
    {
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            CORE_LOG_INFO("Using surface format with sRGB");
            return format.format;
        }
    }
    CORE_LOG_INFO("Using arbitrary surface format");
    return availableFormats[0].format;
}

vk::PresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
    CORE_LOG_INFO("Found {} present modes(s)", availablePresentModes.size());
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            CORE_LOG_INFO("Using present mode MailBox");
            return availablePresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D VulkanSwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    auto resolution = m_Window.GetResolution();
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        CORE_LOG_INFO("Using current extent ({}x{})", capabilities.currentExtent.width, capabilities.currentExtent.height);
        return capabilities.currentExtent;
    }
    CORE_LOG_INFO("Using extent {}x{}", resolution.first, resolution.second);
    return {
        std::clamp<uint32_t>(resolution.first, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(resolution.second, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}
