#pragma once
#include <vector>
#include "Device.h"
#include <vulkan/vulkan_raii.hpp>

namespace LiteVulkan {
class Renderer;
class SwapChain
{
friend class Renderer;
public:
    SwapChain(const Device& device);
    void Reset();
    void CreateSwapChain(const uint32_t width, const uint32_t height, const vk::raii::SurfaceKHR& surface);
    void CreateImageViews();
    const vk::raii::SwapchainKHR& GetSwapChain() const { return m_SwapChain; };
    uint32_t GetExtentWidth() const { return m_Extent.width; };
    uint32_t GetExtentHeight() const { return m_Extent.height; };
    vk::Format GetImageFormat() const { return m_ImageFormat; };
private:
    vk::Format ChooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& aFormats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& aPresentModes);
private:
    vk::raii::SwapchainKHR m_SwapChain = nullptr;
    std::vector<vk::Image> m_Images;
    vk::Format m_ImageFormat = vk::Format::eUndefined;
    vk::Extent2D m_Extent;
    std::vector<vk::raii::ImageView> m_ImageViews;
private:
    const Device& m_DeviceRef;
};
}