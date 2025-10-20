#pragma once
#include "Device.h"
#include "tiny_obj_loader.h"
#include <vulkan/vulkan_raii.hpp>

namespace LiteVulkan {
class Renderer;
class Assets
{
    friend class Renderer;

public:
    Assets(const Renderer& renderer, const Device& device);
    void LoadModel();
    void CreateTexture();
    void CreateColorResources(const uint32_t width, const uint32_t height,
                              const vk::Format colorFormat);
    void CreateDepthResources(const uint32_t width, const uint32_t height);
    void CreateTextureImageView();
    void CreateTextureSampler();
    vk::Format GetDepthFormat() const { return m_DepthFormat; };
    const vk::raii::Sampler& GetSampler() const { return m_TextureSampler; };
    const vk::raii::ImageView& GetTextureImageView() const { return m_TextureImageView; };
    const std::vector<tinyobj::shape_t>& GetShapes() const { return shapes; };
    const tinyobj::attrib_t& GetAttributes() const { return attrib; };

private:
    vk::raii::ImageView GetImageView(vk::raii::Image& img, vk::Format format,
                                     vk::ImageAspectFlags aspect_flags, uint32_t mip_levels);
    void CreateImage(uint32_t width, uint32_t height, uint32_t mip_levels,
                     vk::SampleCountFlagBits nsamples, vk::Format format, vk::ImageTiling tiling,
                     vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
                     vk::raii::Image& image, vk::raii::DeviceMemory& imageMemory);
    void TransitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout,
                               vk::ImageLayout newLayout, uint32_t mip_levels);
    void CopyBufferToImage(const vk::raii::Buffer& buf, vk::raii::Image& image, uint32_t width,
                           uint32_t height);
    void GenerateMipmaps(vk::raii::Image& image, vk::Format imageFormat, int32_t texWidth,
                         int32_t texHeight, uint32_t mipLevels);
    vk::Format FindDepthFormat();
    vk::Format FindSupportedFormat(const std::vector<vk::Format>& candidates,
                                   vk::ImageTiling tiling, vk::FormatFeatureFlags features);

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
    vk::Format m_DepthFormat;

private: // Models
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

private:
    const Device& m_DeviceRef;
    const Renderer& m_RendererRef;
};
}