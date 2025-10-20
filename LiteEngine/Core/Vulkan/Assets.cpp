#include "Assets.h"
#define STB_IMAGE_IMPLEMENTATION
#include "Core/Logging/Logger.h"
#include "Renderer.h"
#include <stb_image.h>

namespace LiteVulkan {
constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;
constexpr uint64_t FenceTimeout = 100000000;
const std::string MODEL_PATH = "Assets/Models/viking_room.obj";
const std::string TEXTURE_PATH = "Assets/Textures/viking_room.png";

Assets::Assets(const Renderer& renderer, const Device& device)
    : m_RendererRef(renderer)
    , m_DeviceRef(device)
{
    m_DepthFormat = FindDepthFormat();
}

void Assets::LoadModel()
{
    std::string warn, err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }
}

void Assets::CreateTexture()
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels
        = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    vk::DeviceSize imageSize = texWidth * texHeight * 4;
    nmip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    vk::raii::Buffer stagingBuffer({});
    vk::raii::DeviceMemory stagingBufferMemory({});
    Buffers::CreateBuffer(m_DeviceRef, imageSize, vk::BufferUsageFlagBits::eTransferSrc,
                          vk::MemoryPropertyFlagBits::eHostVisible
                              | vk::MemoryPropertyFlagBits::eHostCoherent,
                          stagingBuffer, stagingBufferMemory);

    void* data = stagingBufferMemory.mapMemory(0, imageSize);
    memcpy(data, pixels, imageSize);
    stagingBufferMemory.unmapMemory();

    stbi_image_free(pixels);

    CreateImage(texWidth, texHeight, nmip_levels, vk::SampleCountFlagBits::e1,
                vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal,
                vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst
                    | vk::ImageUsageFlagBits::eSampled,
                vk::MemoryPropertyFlagBits::eDeviceLocal, m_TextureImage, m_TextureMemory);

    TransitionImageLayout(m_TextureImage, vk::ImageLayout::eUndefined,
                          vk::ImageLayout::eTransferDstOptimal, nmip_levels);
    CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth),
                      static_cast<uint32_t>(texHeight));
    GenerateMipmaps(m_TextureImage, vk::Format::eR8G8B8A8Srgb, texWidth, texHeight, nmip_levels);
}

void Assets::CreateColorResources(const uint32_t width, const uint32_t height,
                                  const vk::Format colorFormat)
{
    CreateImage(
        width, height, 1, m_DeviceRef.GetMsaaSamples(), colorFormat, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal, m_ColorImage, m_ColorImageMemory);
    m_ColorImageView = GetImageView(m_ColorImage, colorFormat, vk::ImageAspectFlagBits::eColor, 1);
}

void Assets::CreateDepthResources(const uint32_t width, const uint32_t height)
{
    CreateImage(width, height, 1, m_DeviceRef.GetMsaaSamples(), m_DepthFormat,
                vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment,
                vk::MemoryPropertyFlagBits::eDeviceLocal, m_DepthImage, m_DepthBufferMemory);
    m_DepthBufferView
        = GetImageView(m_DepthImage, m_DepthFormat, vk::ImageAspectFlagBits::eDepth, 1);
}

void Assets::CreateTextureImageView()
{
    m_TextureImageView = GetImageView(m_TextureImage, vk::Format::eR8G8B8A8Srgb,
                                      vk::ImageAspectFlagBits::eColor, nmip_levels);
}

vk::raii::ImageView Assets::GetImageView(vk::raii::Image& img, vk::Format format,
                                         vk::ImageAspectFlags aspect_flags, uint32_t mip_levels)
{
    vk::ImageViewCreateInfo info { .image = img,
                                   .viewType = vk::ImageViewType::e2D,
                                   .format = format,
                                   .subresourceRange = { aspect_flags, 0, mip_levels, 0, 1 } };
    return vk::raii::ImageView(m_DeviceRef.GetDevice(), info);
}

void Assets::CreateTextureSampler()
{
    vk::PhysicalDeviceProperties properties = m_DeviceRef.GetPhysicalDevice().getProperties();
    vk::SamplerCreateInfo samplerInfo { .magFilter = vk::Filter::eLinear,
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
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(nmip_levels);

    m_TextureSampler = vk::raii::Sampler(m_DeviceRef.GetDevice(), samplerInfo);
}

void Assets::CreateImage(uint32_t width, uint32_t height, uint32_t mip_levels,
                         vk::SampleCountFlagBits nsamples, vk::Format format,
                         vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                         vk::MemoryPropertyFlags properties, vk::raii::Image& image,
                         vk::raii::DeviceMemory& imageMemory)
{
    vk::ImageCreateInfo imageInfo { .imageType = vk::ImageType::e2D,
                                    .format = format,
                                    .extent = { width, height, 1 },
                                    .mipLevels = mip_levels,
                                    .arrayLayers = 1,
                                    .samples = nsamples,
                                    .tiling = tiling,
                                    .usage = usage,
                                    .sharingMode = vk::SharingMode::eExclusive,
                                    .initialLayout = vk::ImageLayout::eUndefined };

    image = vk::raii::Image(m_DeviceRef.GetDevice(), imageInfo);

    vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo { .allocationSize = memRequirements.size,
                                       .memoryTypeIndex = Buffers::FindMemoryType(
                                           m_DeviceRef.GetPhysicalDevice().getMemoryProperties(),
                                           memRequirements.memoryTypeBits, properties) };
    imageMemory = vk::raii::DeviceMemory(m_DeviceRef.GetDevice(), allocInfo);
    image.bindMemory(imageMemory, 0);
}

void Assets::TransitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout,
                                   vk::ImageLayout newLayout, uint32_t mip_levels)
{
    auto cmdbuf = m_RendererRef.BeginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier { .oldLayout = oldLayout,
                                     .newLayout = newLayout,
                                     .image = image,
                                     .subresourceRange
                                     = { vk::ImageAspectFlagBits::eColor, 0, mip_levels, 0, 1 } };

    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;

    if (oldLayout == vk::ImageLayout::eUndefined
        && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (oldLayout == vk::ImageLayout::eTransferDstOptimal
               && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
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

void Assets::CopyBufferToImage(const vk::raii::Buffer& buf, vk::raii::Image& image, uint32_t width,
                               uint32_t height)
{
    auto cmdbuf = m_RendererRef.BeginSingleTimeCommands();
    vk::BufferImageCopy region { .bufferOffset = 0,
                                 .bufferRowLength = 0,
                                 .bufferImageHeight = 0,
                                 .imageSubresource = { vk::ImageAspectFlagBits::eColor, 0, 0, 1 },
                                 .imageOffset = { 0, 0, 0 },
                                 .imageExtent = { width, height, 1 } };
    cmdbuf->copyBufferToImage(buf, image, vk::ImageLayout::eTransferDstOptimal, { region });
    m_RendererRef.EndSingleTimeCommands(*cmdbuf);
}

void Assets::GenerateMipmaps(vk::raii::Image& image, vk::Format imageFormat, int32_t texWidth,
                             int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blit-ing
    vk::FormatProperties formatProperties
        = m_DeviceRef.GetPhysicalDevice().getFormatProperties(imageFormat);

    if (!(formatProperties.optimalTilingFeatures
          & vk::FormatFeatureFlagBits::eSampledImageFilterLinear)) {
        throw std::runtime_error("texture image format does not support linear blitting!");
    }

    std::unique_ptr<vk::raii::CommandBuffer> commandBuffer
        = m_RendererRef.BeginSingleTimeCommands();

    vk::ImageMemoryBarrier barrier = { .srcAccessMask = vk::AccessFlagBits::eTransferWrite,
                                       .dstAccessMask = vk::AccessFlagBits::eTransferRead,
                                       .oldLayout = vk::ImageLayout::eTransferDstOptimal,
                                       .newLayout = vk::ImageLayout::eTransferSrcOptimal,
                                       .srcQueueFamilyIndex = vk::QueueFamilyIgnored,
                                       .dstQueueFamilyIndex = vk::QueueFamilyIgnored,
                                       .image = image };
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                       vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, barrier);

        vk::ArrayWrapper1D<vk::Offset3D, 2> offsets, dstOffsets;
        offsets[0] = vk::Offset3D(0, 0, 0);
        offsets[1] = vk::Offset3D(mipWidth, mipHeight, 1);
        dstOffsets[0] = vk::Offset3D(0, 0, 0);
        dstOffsets[1]
            = vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1);
        vk::ImageBlit blit = { .srcSubresource = {},
                               .srcOffsets = offsets,
                               .dstSubresource = {},
                               .dstOffsets = dstOffsets };
        blit.srcSubresource
            = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i - 1, 0, 1);
        blit.dstSubresource = vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, i, 0, 1);

        commandBuffer->blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image,
                                 vk::ImageLayout::eTransferDstOptimal, { blit },
                                 vk::Filter::eLinear);

        barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                       vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {},
                                       barrier);

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                   vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, barrier);
    m_RendererRef.EndSingleTimeCommands(*commandBuffer);
}

vk::Format Assets::FindSupportedFormat(const std::vector<vk::Format>& candidates,
                                       vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    for (const auto format : candidates) {
        vk::FormatProperties props = m_DeviceRef.GetPhysicalDevice().getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear
            && (props.linearTilingFeatures & features) == features) {
            return format;
        }
        if (tiling == vk::ImageTiling::eOptimal
            && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("Failed to find supported format");
}

vk::Format Assets::FindDepthFormat()
{
    return FindSupportedFormat(
        { vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
        vk::ImageTiling::eOptimal, vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}
}