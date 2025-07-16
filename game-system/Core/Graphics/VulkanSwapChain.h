#pragma once
#include "VulkanDevice.h"

class VulkanContext;

class VulkanSwapChain
{
  public:
    VulkanSwapChain(VulkanContext *context);
    ~VulkanSwapChain();

  public:
    void CreateSwapchain();
    void CreateImageViews();
    void Destroy();
  public:
    VkFormat GetImageFormat() { return m_ImageFormat; };
  private:
    SwapChainSupportDetails QuerySupport();
    VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &foramts);
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR> &present_modes);
    VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);

  private:
    std::vector<VkImage> m_Images;
    std::vector<VkImageView> m_ImageViews;
    VkFormat m_ImageFormat;
    SwapChainSupportDetails m_SwapchainSupport;
    VkSurfaceFormatKHR m_SurfaceFormat;
    VkPresentModeKHR m_PresentMode;
    VkExtent2D m_Extent;
    uint32_t m_ImageCount;

  private:
    VulkanContext *p_Context;
};