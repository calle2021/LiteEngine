#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "VulkanSwapChain.h"
#include "VulkanDevice.h"
#include "VulkanGraphicsPipeline.h"

class VulkanRenderer
{
public:
    VulkanRenderer(VulkanSwapChain& swapChain, VulkanDevice& device, VulkanGraphicsPipeline& graphicsPipeline);
    void DrawFrame();
    void CreateCommandPool();
    void CreateCommandBuffer();
    void RecordCommandBuffer(uint32_t imageIndex);
    void CreateSyncObjects();
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
private: // References
    VulkanSwapChain& m_VulkanSwapChain;
    VulkanDevice& m_VulkanDevice;
    VulkanGraphicsPipeline& m_GraphicsPipeline;
};