#pragma once
#include <vulkan/vulkan.hpp>

#include <optional>
#include <vector>

class VulkanContext;

struct QueueFamilyIndices
{
    std::optional<uint32_t> m_GraphicsFamily;
    std::optional<uint32_t> m_PresentFamily;
    bool IsComplete()
    {
        return m_GraphicsFamily.has_value() && m_PresentFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_Capabilities;
    std::vector<VkSurfaceFormatKHR> m_Formats;
    std::vector<VkPresentModeKHR> m_PresentModes;
};

class VulkanDevice
{
  public:
    VulkanDevice(VulkanContext *context);
    ~VulkanDevice();

  public:
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    QueueFamilyIndices GetFamilyIndicies()
    {
        return m_QueueIndicies;
    };

  private:
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    bool VulkanDevice::CheckDeviceExtensions(VkPhysicalDevice device);

  private:
    const std::vector<const char *> DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    QueueFamilyIndices m_QueueIndicies;

  private:
    VulkanContext *p_Context;
};