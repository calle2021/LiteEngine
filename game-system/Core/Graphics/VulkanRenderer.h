#pragma once
#include <vulkan/vulkan_raii.hpp>

class VulkanRenderer
{
public:
    void CreateCommandPool(vk::raii::Device& device, uint32_t graphicsIndex);
    void CreateCommandBuffer(vk::raii::Device& device);
    void RecordCommandBuffer(
        std::vector<vk::Image>& swapChainImages,
        std::vector<vk::raii::ImageView>& swapChainImageViews,
        uint32_t imageIndex,
        vk::Extent2D& swapChainExtent,
        vk::raii::Pipeline& graphicsPipeline
    );
    void CreateSyncObjects(vk::raii::Device& device);
private:
    void TransitionImageLayout(
        std::vector<vk::Image>& swapChainImages,
        uint32_t imageIndex,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::AccessFlags2 srcAccessMask,
        vk::AccessFlags2 dstAccessMask,
        vk::PipelineStageFlags2 srcStageMask,
        vk::PipelineStageFlags2 dstStageMask
    );
private:
    vk::raii::CommandPool m_CommandPool = nullptr;
    vk::raii::CommandBuffer m_CommandBuffer = nullptr;
private: // Synchronization objects
    vk::raii::Semaphore presentCompleteSemaphore = nullptr;
    vk::raii::Semaphore renderFinishedSemaphore = nullptr;
    vk::raii::Fence drawFence = nullptr;
};