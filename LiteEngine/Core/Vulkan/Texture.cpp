#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace LiteVulkan
{
Texture::Texture(Device& dev, Buffers& buf, Renderer& rend, SwapChain& swap)
    : m_DeviceRef(dev)
    , m_BuffersRef(buf)
    , m_RendererRef(rend)
    , m_SwapChainRef(swap) {}

void Texture::CreateTexture(std::string path)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});
    m_BuffersRef.CreateBuffer(imageSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, stagingBuffer, stagingBufferMemory);

    void* data = stagingBufferMemory.mapMemory(0, imageSize);
    memcpy(data, pixels, imageSize);
    stagingBufferMemory.unmapMemory();

    stbi_image_free(pixels);

    CreateImage(texWidth, texHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, m_TextureImage, m_TextureMemory);

    TransitionImageLayout(m_TextureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    TransitionImageLayout(m_TextureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void Texture::CreateDepthResources()
{
    vk::Format depthFormat = FindDepthFormat();
    CreateImage(m_SwapChainRef.m_Extent.width, m_SwapChainRef.m_Extent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, m_DepthImage, m_DepthBufferMemory);
    m_DepthBufferView = m_SwapChainRef.GetImageView(m_DepthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

void Texture::CreateTextureImageView()
{
    m_TextureImageView = m_SwapChainRef.GetImageView(m_TextureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}
void Texture::CreateTextureSampler()
{
    vk::PhysicalDeviceProperties properties = m_DeviceRef.m_PhysicalDevice.getProperties();
    vk::SamplerCreateInfo samplerInfo{
        .magFilter = vk::Filter::eLinear,
        .minFilter = vk::Filter::eLinear,
        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat,
        .addressModeV = vk::SamplerAddressMode::eRepeat,
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .mipLodBias = 0.0f,
        .anisotropyEnable = vk::True,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = vk::False,
        .compareOp = vk::CompareOp::eAlways };
    m_TextureSampler = vk::raii::Sampler(m_DeviceRef.m_Device, samplerInfo);
}

void Texture::CreateImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image& image, vk::raii::DeviceMemory& imageMemory) {
    vk::ImageCreateInfo imageInfo{ .imageType = vk::ImageType::e2D, .format = format,
                                    .extent = {width, height, 1}, .mipLevels = 1, .arrayLayers = 1,
                                    .samples = vk::SampleCountFlagBits::e1, .tiling = tiling,
                                    .usage = usage, .sharingMode = vk::SharingMode::eExclusive,
                                    .initialLayout = vk::ImageLayout::eUndefined };

    image = vk::raii::Image(m_DeviceRef.m_Device, imageInfo );

    vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size,
                                     .memoryTypeIndex = m_BuffersRef.FindMemoryType(memRequirements.memoryTypeBits, properties) };
    imageMemory = vk::raii::DeviceMemory(m_DeviceRef.m_Device, allocInfo);
    image.bindMemory(imageMemory, 0);
}

void Texture::TransitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
    auto cmdbuf = m_RendererRef.BeginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier{ .oldLayout = oldLayout, .newLayout = newLayout,
                                    .image = image,
                                    .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 } };

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }
    cmdbuf->pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
    m_RendererRef.EndSingleTimeCommands(*cmdbuf);
}

void Texture::CopyBufferToImage(const vk::raii::Buffer& buf, vk::raii::Image& image, uint32_t width, uint32_t height)
{
    auto cmdbuf = m_RendererRef.BeginSingleTimeCommands();
    vk::BufferImageCopy region{ .bufferOffset = 0, .bufferRowLength = 0, .bufferImageHeight = 0,
                                .imageSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 },
                                .imageOffset = {0, 0, 0}, .imageExtent = {width, height, 1} };
    cmdbuf->copyBufferToImage(buf, image, vk::ImageLayout::eTransferDstOptimal, {region});
    m_RendererRef.EndSingleTimeCommands(*cmdbuf);
}

vk::Format Texture::FindSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    for (const auto format : candidates) {
        vk::FormatProperties props = m_DeviceRef.m_PhysicalDevice.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("Failed to find supported format");
}

vk::Format Texture::FindDepthFormat()
{
    return FindSupportedFormat({vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint}, vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}
}