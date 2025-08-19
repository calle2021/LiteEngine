#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Buffers.h"
#include "SwapChain.h"
#include "Device.h"
#include "Pipeline.h"
#include "Core/Window/GLFWindow.h"

namespace LiteVulkan {
class Buffers;
class Renderer
{
friend class Buffers;
public:
    Renderer(
        Buffers& buffer,
        SwapChain& swap,
        Device& device,
        Pipeline& Pipeline,
        GLFWindow& window
    );
    void DrawFrame();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void RecordCommandBuffer(uint32_t imageIndex);
    void CreateSyncObjects();
    void CreateDescriptorSets();
    void CreateDescriptorPool();
private:
    void TransitionImageLayout(
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
    std::vector<vk::raii::CommandBuffer> m_CommandBuffers;
    vk::raii::DescriptorPool m_DescriptorPool = nullptr;
    std::vector<vk::raii::DescriptorSet> m_DescriptorSets;
private: // Synchronization objects
    std::vector<vk::raii::Semaphore> m_PresentSemaphores;
    std::vector<vk::raii::Semaphore> m_RenderSemaphores;
    std::vector<vk::raii::Fence> m_Fences;
private: // References
    Buffers& m_Buffers;
    SwapChain& m_SwapChain;
    Device& m_Device;
    Pipeline& m_Pipeline;
    GLFWindow& m_Window;
private:
    uint32_t m_CurrentFrame = 0;
    const uint32_t MaxFramesInFlight = 2;
};
}