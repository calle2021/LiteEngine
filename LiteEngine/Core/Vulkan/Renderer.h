#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "VertexBuffer.h"
#include "SwapChain.h"
#include "Device.h"
#include "GraphicsPipeline.h"
#include "Core/Window/GLFWindow.h"

namespace LiteVulkan {
class Renderer
{
public:
    Renderer(
        VertexBuffer& vertexBuffer,
        SwapChain& swapChain,
        Device& device,
        GraphicsPipeline& graphicsPipeline,
        GLFWindow& window
    );
    void DrawFrame();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void RecordCommandBuffer(uint32_t imageIndex);
    void CreateSyncObjects();
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
private: // Synchronization objects
    std::vector<vk::raii::Semaphore> m_PresentSemaphores;
    std::vector<vk::raii::Semaphore> m_RenderSemaphores;
    std::vector<vk::raii::Fence> m_Fences;
private: // References
    VertexBuffer& m_VertexBuffer;
    SwapChain& m_SwapChain;
    Device& m_Device;
    GraphicsPipeline& m_GraphicsPipeline;
    GLFWindow& m_Window;
private:
    uint32_t m_CurrentFrame = 0;
};
}