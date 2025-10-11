#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Window/Window.h"
#include "Device.h"

namespace LiteVulkan {
class Pipeline;
class Renderer;
class Buffers;
class Assets;
class SwapChain
{
friend class Pipeline;
friend class Renderer;
friend class Buffers;
friend class Assets;
public:
    SwapChain(
        Device& device,
        vk::raii::SurfaceKHR& surface,
        LiteEngine::Window& window
    );
    void CreateSwapChain();
    void CreateImageViews();
private:
    vk::raii::ImageView GetImageView(vk::raii::Image& img, vk::Format format,
                                     vk::ImageAspectFlags aspect_flags, uint32_t mip_levels);
    vk::Format ChooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D ChooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
private:
    vk::raii::SwapchainKHR m_SwapChain = nullptr;
    std::vector<vk::Image> m_Images;
    vk::Format m_ImageFormat = vk::Format::eUndefined;
    vk::Extent2D m_Extent;
    std::vector<vk::raii::ImageView> m_ImageViews;
private:
    Device& m_Device;
    vk::raii::SurfaceKHR& m_Surface;
    LiteEngine::Window& m_Window;
};
}