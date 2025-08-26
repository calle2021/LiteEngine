#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace LiteVulkan
{
Texture::Texture(Device& dev, Buffers& buf, Renderer& rend)
    : m_DeviceRef(dev)
    , m_BuffersRef(buf)
    , m_RendererRef(rend) {}

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

    CreateImage(texWidth, texHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, m_Texture, m_TextureMemory);

    TransitionImageLayout(m_Texture, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
    CopyBufferToImage(stagingBuffer, m_Texture, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    TransitionImageLayout(m_Texture, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void Texture::CreateTextureImageView()
{
    vk::ImageViewCreateInfo viewInfo
    {
        .image = m_Texture,
        .viewType = vk::ImageViewType::e2D,
        .format = vk::Format::eR8G8B8A8Srgb,
        .subresourceRange = { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
    };
    m_TextureImageView = vk::raii::ImageView(m_DeviceRef.m_Device, viewInfo);
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
                                    .usage = usage, .sharingMode = vk::SharingMode::eExclusive };

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
}