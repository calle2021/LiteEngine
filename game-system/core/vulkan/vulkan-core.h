#pragma once
#include <vulkan/vulkan.h>

namespace GameSystem
{
class VulkanCore
{
  public:
    static void Init();
    static void Destroy();

  private:
    static VkInstance vk_instance;
    static VkPhysicalDevice vk_physical_device;
    static VkDevice vk_device;
    static VkQueue vk_graphics_queue;
    static VkSurfaceKHR vk_surface;
    static VkQueue vk_present_queue;
    static VkSwapchainKHR vk_swap_chain;
    static std::vector<VkImage> swapChainImages;
    static VkFormat swapChainImageFormat;
    static VkExtent2D swapChainExtent;
    static std::vector<VkImageView> swapChainImageViews;
};
} // namespace GameSystem