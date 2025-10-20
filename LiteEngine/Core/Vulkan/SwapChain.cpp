#include "SwapChain.h"
#include "Core/Logging/Logger.h"

namespace LiteVulkan {

const uint32_t prefImageCount = 3;

SwapChain::SwapChain(const Device& device) : m_DeviceRef(device){}

void SwapChain::Reset() {
    m_ImageViews.clear();
    m_SwapChain = nullptr;
}

void SwapChain::CreateSwapChain(const uint32_t width, const uint32_t height, const vk::raii::SurfaceKHR& surface)
{
    auto sCapabilities = m_DeviceRef.GetPhysicalDevice().getSurfaceCapabilitiesKHR(surface);
    m_ImageFormat = ChooseSwapchainSurfaceFormat(m_DeviceRef.GetPhysicalDevice().getSurfaceFormatsKHR(surface));
    m_Extent = {
        std::clamp<uint32_t>(width, sCapabilities.minImageExtent.width, sCapabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, sCapabilities.minImageExtent.height, sCapabilities.maxImageExtent.height)
    };
    uint32_t minImageCount = (prefImageCount > sCapabilities.maxImageCount) ? sCapabilities.maxImageCount : prefImageCount;

    vk::SwapchainCreateInfoKHR swapCreateInfo = {};
    swapCreateInfo.surface = surface;
    swapCreateInfo.minImageCount = sCapabilities.minImageCount;
    swapCreateInfo.imageFormat = m_ImageFormat;
    swapCreateInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    swapCreateInfo.imageExtent = m_Extent;
    swapCreateInfo.imageArrayLayers =1;
    swapCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
    swapCreateInfo.preTransform = sCapabilities.currentTransform;
    swapCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapCreateInfo.presentMode = ChooseSwapPresentMode(m_DeviceRef.GetPhysicalDevice().getSurfacePresentModesKHR(surface));
    swapCreateInfo.clipped = true;

    m_SwapChain = vk::raii::SwapchainKHR(m_DeviceRef.GetDevice(), swapCreateInfo);
    m_Images = m_SwapChain.getImages();
    CORE_LOG_INFO("Swap chain created");
}

void SwapChain::CreateImageViews() {
    m_ImageViews.clear();
    vk::ImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
    imageViewCreateInfo.format = m_ImageFormat;
    imageViewCreateInfo.subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 };
    for ( auto image : m_Images ) {
        imageViewCreateInfo.image = image;
        m_ImageViews.emplace_back(m_DeviceRef.GetDevice(), imageViewCreateInfo);
    }
}

vk::Format SwapChain::ChooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& aFormats) {
    for(const auto& format : aFormats) {
        // Prefer sRGB
        if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            return format.format;
        }
    }
    CORE_LOG_WARN("Using arbitrary surface format");
    return aFormats[0].format;
}

vk::PresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& aPresentModes) {
    for (const auto& aPresentMode : aPresentModes) {
         // Prefer Mailbox
        if (aPresentMode == vk::PresentModeKHR::eMailbox) {
            return aPresentMode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}
}