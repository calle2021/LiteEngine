#pragma once
#include "vulkan-core.h"
#include <optional>
#include <vulkan/vulkan.h>

namespace GameSystem
{

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphics_family;
    bool IsComplete()
    {
        return graphics_family.has_value();
    }
};

class VulkanUtils
{
  public:
    static std::vector<const char *> GetRequiredExtenstions();
    static bool CheckValidationLayers();
    static bool IsDeviceSuitable(VkPhysicalDevice device);
    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    static void Destroy();

  private:
    static const std::vector<const char *> validation_layers;
};
} // namespace GameSystem