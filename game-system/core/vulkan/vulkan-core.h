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
    static VkPhysicalDevice physical_device;
    static VkDevice vk_device;
    static VkQueue vk_graphics_q;
};
} // namespace GameSystem