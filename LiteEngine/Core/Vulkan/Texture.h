#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Device.h"
#include "Buffers.h"
#include "Renderer.h"

namespace LiteVulkan
{
class Texture
{
public:
    Texture(Device& dev, Buffers& buf, Renderer& rend);
    void CreateTexture(std::string path);
private:
    void CreateImage(uint32_t width, uint32_t height, vk::Format format,
                    vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                    vk::MemoryPropertyFlags properties, vk::raii::Image& image,
                    vk::raii::DeviceMemory& imageMemory);
    void TransitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void CopyBufferToImage(const vk::raii::Buffer& buf, vk::raii::Image& image, uint32_t width, uint32_t height);
private:
    vk::raii::Image m_Texture = nullptr;
    vk::raii::DeviceMemory m_TextureMemory = nullptr;
private:
    Device& m_DeviceRef;
    Buffers& m_BuffersRef;
    Renderer& m_RendererRef;
};
}