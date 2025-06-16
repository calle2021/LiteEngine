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
};
} // namespace GameSystem