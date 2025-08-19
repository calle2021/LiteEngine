#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Window/GLFWindow.h"
#include "Device.h"

namespace LiteVulkan {
class Pipeline;
class Renderer;
class Buffers;
class SwapChain
{
friend class Pipeline;
friend class Renderer;
friend class Buffers;
public:
    SwapChain(
        Device& device,
        vk::raii::SurfaceKHR& surface,
        GLFWindow& window
    );
    void CreateSwapChain();
    void CreateImageViews();
    void RecreateSwapChain();
private:
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
    GLFWindow& m_Window;
};
}