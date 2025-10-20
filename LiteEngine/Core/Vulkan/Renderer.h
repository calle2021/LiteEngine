#pragma once
#include "Assets.h"
#include "Buffers.h"
#include "Core/Camera/Camera.h"
#include "Core/Window/Window.h"
#include "Device.h"
#include "Pipeline.h"
#include "SwapChain.h"
#include <memory>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_raii.hpp>

using namespace LiteEngine;

namespace LiteVulkan {
class Renderer
{
public:
    Renderer(Window& window, const Camera& camera);
    void Init();
    void DrawFrame();
    void Shutdown();

public:
    std::unique_ptr<vk::raii::CommandBuffer> BeginSingleTimeCommands() const;
    void EndSingleTimeCommands(vk::raii::CommandBuffer& cmdbuf) const;

private:
    [[nodiscard]] vk::raii::Instance CreateInstance();
    [[nodiscard]] vk::raii::DebugUtilsMessengerEXT SetupDebugMessenger();
    [[nodiscard]] vk::raii::SurfaceKHR CreateSurface();
    [[nodiscard]] vk::raii::CommandPool CreateCommandPool();
    [[nodiscard]] std::vector<vk::raii::CommandBuffer> CreateCommandBuffers();
    void CreateSyncObjects();
    void RecreateSwapChain();
    void RecordCommandBuffer(uint32_t imageIndex);
    template <typename T>
    void TransitionImageLayout(const T& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                               vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
                               vk::PipelineStageFlags2 srcStageMask,
                               vk::PipelineStageFlags2 dstStageMask,
                               vk::ImageAspectFlags aspect_mask);

private:
    vk::raii::Context m_Context;
    vk::raii::Instance m_Instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    vk::raii::SurfaceKHR m_Surface = nullptr;
    vk::raii::CommandPool m_CommandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_CommandBuffers;

private: // Synchronization objects
    std::vector<vk::raii::Semaphore> m_PresentSemaphores;
    std::vector<vk::raii::Semaphore> m_RenderSemaphores;
    std::vector<vk::raii::Fence> m_Fences;
    uint32_t m_CurrentFrame = 0;

private:
    Window& m_Window;
    const LiteEngine::Camera& m_Camera;

private: // Subclasses
    std::unique_ptr<Device> m_Device;
    std::unique_ptr<SwapChain> m_SwapChain;
    std::unique_ptr<Pipeline> m_Pipeline;
    std::unique_ptr<Buffers> m_Buffers;
    std::unique_ptr<Assets> m_Assets;
};
}