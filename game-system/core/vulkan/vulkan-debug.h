#pragma once
#include <vulkan/vulkan.h>

namespace GameSystem
{
class VulkanDebug
{
  public:
    static std::vector<const char *> GetRequiredExtenstions();
    static bool CheckValidationLayers();
    static void SetupDebugMessenger();

  private:
    static VkDebugUtilsMessengerEXT debug_messenger;
};
} // namespace GameSystem