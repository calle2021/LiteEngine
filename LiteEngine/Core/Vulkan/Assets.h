#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Device.h"
#include "Buffers.h"
#include "Renderer.h"
#include "SwapChain.h"

#include "tiny_obj_loader.h"

class Renderer;
class Pipeline;
class Buffers;
namespace LiteVulkan
{
class Assets
{
friend class Pipeline;
friend class Renderer;
friend class Buffers;
public:
    Assets(Device& dev, Buffers& buf, Renderer& rend, SwapChain& swap);
    void LoadModel();
    void CreateTexture();
    void CreateColorResources();
    void CreateDepthResources();
    void CreateTextureImageView();
    void CreateTextureSampler();
private:
    void CreateImage(uint32_t width, uint32_t height, uint32_t mip_levels, vk::SampleCountFlagBits nsamples,
                    vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                    vk::MemoryPropertyFlags properties, vk::raii::Image& image,
                    vk::raii::DeviceMemory& imageMemory);
    void TransitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout, uint32_t mip_levels);
    void CopyBufferToImage(const vk::raii::Buffer& buf, vk::raii::Image& image, uint32_t width, uint32_t height);
    void GenerateMipmaps(vk::raii::Image& image, vk::Format imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    vk::Format FindDepthFormat();
    vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
private:
    uint32_t nmip_levels;
    vk::raii::Image m_TextureImage = nullptr;
    vk::raii::DeviceMemory m_TextureMemory = nullptr;
    vk::raii::ImageView m_TextureImageView = nullptr;
    vk::raii::Sampler m_TextureSampler = nullptr;
    vk::raii::Image m_DepthImage = nullptr;
    vk::raii::DeviceMemory m_DepthBufferMemory = nullptr;
    vk::raii::ImageView m_DepthBufferView = nullptr;
    vk::raii::Image m_ColorImage = nullptr;
    vk::raii::DeviceMemory m_ColorImageMemory = nullptr;
    vk::raii::ImageView m_ColorImageView = nullptr;

private: // Models
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
private:
    Device& m_DeviceRef;
    Buffers& m_BuffersRef;
    Renderer& m_RendererRef;
    SwapChain& m_SwapChainRef;
};
}