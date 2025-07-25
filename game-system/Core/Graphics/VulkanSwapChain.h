#pragma once
#include <vector>
#include <vulkan/vulkan_raii.hpp>
#include "Core/Window/GLFWindow.h"

class VulkanSwapChain
{
public:
    void CreateSwapchain(std::pair<uint32_t, uint32_t> resolution, vk::raii::Device& device, vk::raii::PhysicalDevice& physicalDevice, vk::raii::SurfaceKHR& surface);
    void CreateImageViews(vk::raii::Device& device);
private:
    vk::Format ChooseSwapchainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
    vk::PresentModeKHR ChooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
    vk::Extent2D ChooseSwapExtent(std::pair<uint32_t, uint32_t> resolution, const vk::SurfaceCapabilitiesKHR& capabilities);
private:
    vk::raii::SwapchainKHR m_Swapchain = nullptr;
    std::vector<vk::Image> m_Images;
    vk::Format m_ImageFormat = vk::Format::eUndefined;
    vk::Extent2D m_Extent;
    std::vector<vk::raii::ImageView> m_ImageViews;
};
