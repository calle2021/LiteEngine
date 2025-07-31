#include "VulkanRenderer.h"
#include "Core/Logging/Logger.h"

void VulkanRenderer::CreateCommandPool(vk::raii::Device& device, uint32_t graphicsIndex)
{
    vk::CommandPoolCreateInfo poolInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = graphicsIndex
    };
    m_CommandPool = vk::raii::CommandPool(device, poolInfo);
    CORE_LOG_INFO("Commandpool created.");
}

void VulkanRenderer::CreateCommandBuffer(vk::raii::Device& device)
{
    vk::CommandBufferAllocateInfo allocInfo {
        .commandPool = m_CommandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    m_CommandBuffer = std::move(vk::raii::CommandBuffers(device, allocInfo).front());
    CORE_LOG_INFO("Commandbuffer created.");
}

void VulkanRenderer::RecordCommandBuffer(
    std::vector<vk::Image>& swapChainImages,
    std::vector<vk::raii::ImageView>& swapChainImageViews,
    uint32_t imageIndex,
    vk::Extent2D& swapChainExtent,
    vk::raii::Pipeline& graphicsPipeline)
{
    m_CommandBuffer.begin({});
    TransitionImageLayout(
        swapChainImages,
        imageIndex,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eTopOfPipe,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput
    );

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::RenderingAttachmentInfo attachmentInfo = {
        .imageView = swapChainImageViews[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor
    };
    vk::RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = swapChainExtent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo
    };

    m_CommandBuffer.beginRendering(renderingInfo);
    m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline);
    m_CommandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f));
    m_CommandBuffer.setScissor( 0, vk::Rect2D( vk::Offset2D( 0, 0 ), swapChainExtent ) );
    m_CommandBuffer.draw(3, 1, 0, 0);
    m_CommandBuffer.endRendering();

    TransitionImageLayout(
        swapChainImages,
        imageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe
    );
    m_CommandBuffer.end();
}

void VulkanRenderer::TransitionImageLayout(
    std::vector<vk::Image>& swapChainImages,
    uint32_t imageIndex,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    vk::AccessFlags2 srcAccessMask,
    vk::AccessFlags2 dstAccessMask,
    vk::PipelineStageFlags2 srcStageMask,
    vk::PipelineStageFlags2 dstStageMask)
{
    vk::ImageMemoryBarrier2 barrier = {
        .srcStageMask = srcStageMask,
        .srcAccessMask = srcAccessMask,
        .dstStageMask = dstStageMask,
        .dstAccessMask = dstAccessMask,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = swapChainImages[imageIndex],
        .subresourceRange = {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1
        }
    };
    vk::DependencyInfo dependencyInfo = {
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };
    m_CommandBuffer.pipelineBarrier2(dependencyInfo);
}
