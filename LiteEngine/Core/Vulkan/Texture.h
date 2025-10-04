#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Device.h"
#include "Buffers.h"
#include "Renderer.h"
#include "SwapChain.h"

class Renderer;
class Pipeline;
namespace LiteVulkan
{
class Texture
{
friend class Pipeline;
friend class Renderer;
public:
    Texture(Device& dev, Buffers& buf, Renderer& rend, SwapChain& swap);
    void CreateTexture(std::string path);
    void CreateDepthResources();
    void CreateTextureImageView();
    void CreateTextureSampler();
private:
    void CreateImage(uint32_t width, uint32_t height, vk::Format format,
                    vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                    vk::MemoryPropertyFlags properties, vk::raii::Image& image,
                    vk::raii::DeviceMemory& imageMemory);
    void TransitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void CopyBufferToImage(const vk::raii::Buffer& buf, vk::raii::Image& image, uint32_t width, uint32_t height);
    vk::Format FindDepthFormat();
    vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
private:
    vk::raii::Image m_TextureImage = nullptr;
    vk::raii::DeviceMemory m_TextureMemory = nullptr;
    vk::raii::ImageView m_TextureImageView = nullptr;
    vk::raii::Sampler m_TextureSampler = nullptr;
    vk::raii::Image m_DepthImage = nullptr;
    vk::raii::DeviceMemory m_DepthBufferMemory = nullptr;
    vk::raii::ImageView m_DepthBufferView = nullptr;
private:
    Device& m_DeviceRef;
    Buffers& m_BuffersRef;
    Renderer& m_RendererRef;
    SwapChain& m_SwapChainRef;
};
}