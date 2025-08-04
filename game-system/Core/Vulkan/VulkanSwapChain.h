#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Window/GLFWindow.h"
#include "VulkanDevice.h"

class VulkanGraphicsPipeline;
class VulkanRenderer;

class VulkanSwapChain
{
friend class VulkanGraphicsPipeline;
friend class VulkanRenderer;
public:
    VulkanSwapChain(VulkanDevice& device);
    void CreateSwapchain(std::pair<uint32_t, uint32_t> resolution, vk::raii::SurfaceKHR& surface);
    void CreateImageViews();
private:
    vk::Format ChooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D ChooseSwapExtent(std::pair<uint32_t, uint32_t> resolution, const vk::SurfaceCapabilitiesKHR& capabilities);
private:
    vk::raii::SwapchainKHR m_SwapChain = nullptr;
    std::vector<vk::Image> m_Images;
    vk::Format m_ImageFormat = vk::Format::eUndefined;
    vk::Extent2D m_Extent;
    std::vector<vk::raii::ImageView> m_ImageViews;
private:
    VulkanDevice& m_VulkanDevice;
};
