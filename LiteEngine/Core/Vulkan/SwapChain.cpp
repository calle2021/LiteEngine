#include "SwapChain.h"
#include "Core/Logging/Logger.h"

namespace LiteVulkan {

const uint32_t preferred_image_count = 3;

SwapChain::SwapChain(Device& device, vk::raii::SurfaceKHR& surface, GLFWindow& window)
    : m_Device(device)
    , m_Surface(surface)
    , m_Window(window) {}

void SwapChain::CreateSwapChain()
{
    auto surfaceCapabilities = m_Device.m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);
    m_ImageFormat = ChooseSwapchainSurfaceFormat(m_Device.m_PhysicalDevice.getSurfaceFormatsKHR(m_Surface));
    m_Extent = ChooseSwapExtent(surfaceCapabilities);
    uint32_t minImageCount = preferred_image_count > surfaceCapabilities.maxImageCount
                             ? surfaceCapabilities.maxImageCount : preferred_image_count;
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{
        .surface = m_Surface,
        .minImageCount = surfaceCapabilities.minImageCount,
        .imageFormat = m_ImageFormat,
        .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
        .imageExtent = m_Extent,
        .imageArrayLayers =1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = surfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = ChooseSwapPresentMode(m_Device.m_PhysicalDevice.getSurfacePresentModesKHR(m_Surface)),
        .clipped = true };

    m_SwapChain = vk::raii::SwapchainKHR(m_Device.m_Device, swapChainCreateInfo);
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
        m_ImageViews.emplace_back(m_Device.m_Device, imageViewCreateInfo);
    }
}

vk::raii::ImageView SwapChain::GetImageView(vk::raii::Image& img, vk::Format format, vk::ImageAspectFlags flags)
{
    vk::ImageViewCreateInfo info {
        .image = img,
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange = { flags, 0, 1, 0, 1 }
    };
    return vk::raii::ImageView(m_Device.m_Device, info);
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

vk::Extent2D SwapChain::ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
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
}