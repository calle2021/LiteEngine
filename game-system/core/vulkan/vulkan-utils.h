#pragma once
#include "vulkan-core.h"
#include <optional>
#include <vulkan/vulkan.h>

namespace GameSystem
{

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    std::optional<uint32_t> present_family;
    bool IsComplete()
    {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_modes;
};

class VulkanUtils
{
  public:
    static std::vector<const char *> GetRequiredExtenstions();
    static bool CheckValidationLayers();
    static bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
    static bool CheckDeviceExtensions(VkPhysicalDevice device);
    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
    static void Destroy();

  public:
    static const std::vector<const char *> VulkanUtils::device_extenstions;

  private:
    static const std::vector<const char *> validation_layers;
};
} // namespace GameSystem