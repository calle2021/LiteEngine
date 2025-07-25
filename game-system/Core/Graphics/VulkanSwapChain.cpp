#include "VulkanSwapChain.h"
#include "Core/Logging/Logger.h"

void VulkanSwapChain::CreateSwapchain(std::pair<uint32_t, uint32_t> resolution, vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::SurfaceKHR& surface)
{
    auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface);
    m_ImageFormat = ChooseSwapchainSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(surface));
    m_Extent = ChooseSwapExtent(resolution, surfaceCapabilities);
    auto minImageCount = std::max( 3u, surfaceCapabilities.minImageCount );
    minImageCount = ( surfaceCapabilities.maxImageCount > 0 && minImageCount > surfaceCapabilities.maxImageCount ) ? surfaceCapabilities.maxImageCount : minImageCount;
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = surface, .minImageCount = minImageCount,
        .imageFormat = m_ImageFormat, .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = m_Extent, .imageArrayLayers =1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment, .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform, .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = ChooseSwapPresentMode(physicalDevice.getSurfacePresentModesKHR(surface)),
        .clipped = true };

    m_Swapchain = vk::raii::SwapchainKHR(device, swapChainCreateInfo);
    CORE_LOG_INFO("Swap chain created.");
    m_Images = m_Swapchain.getImages();
}

void VulkanSwapChain::CreateImageViews(vk::raii::Device& device)
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
        m_ImageViews.emplace_back(device, imageViewCreateInfo);
    }
    CORE_LOG_INFO("Image views created.");
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

vk::Extent2D VulkanSwapChain::ChooseSwapExtent(std::pair<uint32_t, uint32_t> resolution, const vk::SurfaceCapabilitiesKHR& capabilities)
{
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
