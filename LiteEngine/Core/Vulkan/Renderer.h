#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Buffers.h"
#include "SwapChain.h"
#include "Device.h"
#include "Pipeline.h"
#include "Assets.h"
#include "Core/Window/Window.h"

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
        Assets& assets,
        LiteEngine::Window& window
    );
    void DrawFrame();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void RecordCommandBuffer(uint32_t imageIndex);
    void CreateSyncObjects();
    std::unique_ptr<vk::raii::CommandBuffer> BeginSingleTimeCommands();
    void EndSingleTimeCommands(vk::raii::CommandBuffer& cmdbuf);
private:
    void RecreateSwapChain();
    template <typename T>
    void TransitionImageLayout(
        const T& image,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout,
        vk::AccessFlags2 srcAccessMask,
        vk::AccessFlags2 dstAccessMask,
        vk::PipelineStageFlags2 srcStageMask,
        vk::PipelineStageFlags2 dstStageMask,
        vk::ImageAspectFlags aspect_mask
    );
private:
    vk::raii::CommandPool m_CommandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_CommandBuffers;
private: // Synchronization objects
    std::vector<vk::raii::Semaphore> m_PresentSemaphores;
    std::vector<vk::raii::Semaphore> m_RenderSemaphores;
    std::vector<vk::raii::Fence> m_Fences;
private: // References
    Buffers& m_Buffers;
    SwapChain& m_SwapChain;
    Device& m_DeviceRef;
    Pipeline& m_Pipeline;
    LiteEngine::Window& m_Window;
    Assets& m_AssetsRef;
private:
    uint32_t m_CurrentFrame = 0;
};
}