#pragma once
#include <memory>
#include <vulkan/vulkan_raii.hpp>
#include <vulkan/vk_platform.h>
#include "Core/Window/Window.h"
#include "Device.h"
#include "SwapChain.h"
#include "Pipeline.h"
#include "Buffers.h"
#include "Assets.h"
#include "Core/Camera/Camera.h"

using namespace LiteEngine;

namespace LiteVulkan {
class Renderer
{
public:
    Renderer(Window& window, const Camera& cam);
    void Init();
    void DrawFrame();
    void Shutdown();
private:
    [[nodiscard]] vk::raii::Instance CreateInstance();
    [[nodiscard]] vk::raii::DebugUtilsMessengerEXT SetupDebugMessenger();
    [[nodiscard]] vk::raii::SurfaceKHR CreateSurface();
private:
    vk::raii::CommandPool CreateCommandPool();
    std::vector<vk::raii::CommandBuffer> CreateCommandBuffers();
    void CreateSyncObjects();
    void RecreateSwapChain();
    void RecordCommandBuffer(uint32_t imageIndex);
    template <typename T>
    void TransitionImageLayout(const T& image, vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout, vk::AccessFlags2 srcAccessMask,
                               vk::AccessFlags2 dstAccessMask, vk::PipelineStageFlags2 srcStageMask,
                               vk::PipelineStageFlags2 dstStageMask, vk::ImageAspectFlags aspect_mask);
public: std::unique_ptr<vk::raii::CommandBuffer> BeginSingleTimeCommands() const;
public: void EndSingleTimeCommands(vk::raii::CommandBuffer& cmdbuf) const;
private:
    vk::raii::CommandPool m_CommandPool = nullptr;
    std::vector<vk::raii::CommandBuffer> m_CommandBuffers;
private: // Synchronization objects
    std::vector<vk::raii::Semaphore> m_PresentSemaphores;
    std::vector<vk::raii::Semaphore> m_RenderSemaphores;
    std::vector<vk::raii::Fence> m_Fences;
    uint32_t m_CurrentFrame = 0;
private:
    vk::raii::Context m_Context;
    vk::raii::Instance m_Instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    vk::raii::SurfaceKHR m_Surface = nullptr;
private:
    Window& m_Window;
    const LiteEngine::Camera& m_Camera;
private:
    std::unique_ptr<Device> m_Device;
    std::unique_ptr<SwapChain> m_SwapChain;
    std::unique_ptr<Pipeline> m_Pipeline;
    std::unique_ptr<Buffers> m_Buffers;
    std::unique_ptr<Assets> m_Assets;
};
}