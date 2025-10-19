#include "SwapChain.h"
#include "Core/Logging/Logger.h"

namespace LiteVulkan {

const uint32_t preferred_image_count = 3;

SwapChain::SwapChain(const Device& device)
    : m_DeviceRef(device){}

void SwapChain::Reset() {
    m_ImageViews.clear();
    m_SwapChain = nullptr;
}

void SwapChain::CreateSwapChain(const uint32_t width, const uint32_t height, const vk::raii::SurfaceKHR& surface)
{
    auto surfaceCapabilities = m_DeviceRef.GetPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
    m_ImageFormat = ChooseSwapchainSurfaceFormat(m_DeviceRef.GetPhysicalDevice().getSurfaceFormatsKHR(surface));
    m_Extent = {
        std::clamp<uint32_t>(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height)
    };
    uint32_t minImageCount = preferred_image_count > surfaceCapabilities.maxImageCount
                             ? surfaceCapabilities.maxImageCount : preferred_image_count;
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = surface,
        .minImageCount = surfaceCapabilities.minImageCount,
        .imageFormat = m_ImageFormat,
        .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = m_Extent,
        .imageArrayLayers =1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = ChooseSwapPresentMode(m_DeviceRef.GetPhysicalDevice().getSurfacePresentModesKHR(surface)),
        .clipped = true };

    m_SwapChain = vk::raii::SwapchainKHR(m_DeviceRef.GetDevice(), swapChainCreateInfo);
    CORE_LOG_INFO("Swap chain created");
    m_Images = m_SwapChain.getImages();
    CORE_LOG_INFO("Using {} images", m_Images.size());
}

void SwapChain::CreateImageViews()
{
    m_ImageViews.clear();

    vk::ImageViewCreateInfo imageViewCreateInfo {
        .viewType = vk::ImageViewType::e2D,
        .format = m_ImageFormat,
        .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
    };

    for ( auto image : m_Images )
    {
        imageViewCreateInfo.image = image;
        m_ImageViews.emplace_back(m_DeviceRef.GetDevice(), imageViewCreateInfo);
    }
}

vk::Format SwapChain::ChooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
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

vk::PresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
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
}