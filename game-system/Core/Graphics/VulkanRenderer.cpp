#include "VulkanRenderer.h"
#include "Core/Logging/Logger.h"


VulkanRenderer::VulkanRenderer(VulkanSwapChain& swapChain, VulkanDevice& device,
                               VulkanGraphicsPipeline& graphicsPipeline)
    : m_VulkanSwapChain(swapChain)
    , m_VulkanDevice(device)
    , m_GraphicsPipeline(graphicsPipeline)
    {}

void VulkanRenderer::DrawFrame()
{
    m_VulkanDevice.m_PresentQueue.waitIdle();

    auto [result, imageIndex] = m_VulkanSwapChain.m_Swapchain.acquireNextImage( UINT64_MAX, *presentCompleteSemaphore, nullptr );
    RecordCommandBuffer(imageIndex);

    m_VulkanDevice.m_Device.resetFences(*drawFence);
    vk::PipelineStageFlags waitDestinationStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
    const vk::SubmitInfo submitInfo{ .waitSemaphoreCount = 1, .pWaitSemaphores = &*presentCompleteSemaphore,
                        .pWaitDstStageMask = &waitDestinationStageMask, .commandBufferCount = 1, .pCommandBuffers = &*m_CommandBuffer,
                        .signalSemaphoreCount = 1, .pSignalSemaphores = &*renderFinishedSemaphore };
    m_VulkanDevice.m_GraphicsQueue.submit(submitInfo, *drawFence);
    while ( vk::Result::eTimeout == m_VulkanDevice.m_Device.waitForFences( *drawFence, vk::True, UINT64_MAX ) )
        ;

    const vk::PresentInfoKHR presentInfoKHR {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*renderFinishedSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &*m_VulkanSwapChain.m_Swapchain,
        .pImageIndices = &imageIndex
    };

    result = m_VulkanDevice.m_PresentQueue.presentKHR(presentInfoKHR);
    switch (result)
    {
        case vk::Result::eSuccess: break;
        case vk::Result::eSuboptimalKHR: std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n"; break;
        default:
        CORE_LOG_ERROR("PresentKHR returned unexpected result.");
        break;  // an unexpected result is returned!
    }
}

void VulkanRenderer::CreateCommandPool()
{
    vk::CommandPoolCreateInfo poolInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = m_VulkanDevice.m_GraphicsIndex.value()
    };
    m_CommandPool = vk::raii::CommandPool(m_VulkanDevice.m_Device, poolInfo);
    CORE_LOG_INFO("Commandpool created with graphics index {}.", poolInfo.queueFamilyIndex);
}

void VulkanRenderer::CreateCommandBuffer()
{
    vk::CommandBufferAllocateInfo allocInfo {
        .commandPool = m_CommandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };
    m_CommandBuffer = std::move(vk::raii::CommandBuffers(m_VulkanDevice.m_Device, allocInfo).front());
    CORE_LOG_INFO("Commandbuffer created.");
}

void VulkanRenderer::CreateSyncObjects()
{
    presentCompleteSemaphore =vk::raii::Semaphore(m_VulkanDevice.m_Device, vk::SemaphoreCreateInfo());
    renderFinishedSemaphore = vk::raii::Semaphore(m_VulkanDevice.m_Device, vk::SemaphoreCreateInfo());
    drawFence = vk::raii::Fence(m_VulkanDevice.m_Device, {.flags = vk::FenceCreateFlagBits::eSignaled});
    CORE_LOG_INFO("Sync objects created.");
}

void VulkanRenderer::RecordCommandBuffer(uint32_t imageIndex)
{
    m_CommandBuffer.begin({});
    TransitionImageLayout(
        m_VulkanSwapChain.m_Images,
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
        .imageView = m_VulkanSwapChain.m_ImageViews[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor
    };
    vk::RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = m_VulkanSwapChain.m_Extent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo
    };

    m_CommandBuffer.beginRendering(renderingInfo);
    m_CommandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *m_GraphicsPipeline.m_GraphicsPipeline);
    m_CommandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_VulkanSwapChain.m_Extent.width), static_cast<float>(m_VulkanSwapChain.m_Extent.height), 0.0f, 1.0f));
    m_CommandBuffer.setScissor( 0, vk::Rect2D( vk::Offset2D( 0, 0 ), m_VulkanSwapChain.m_Extent ) );
    m_CommandBuffer.draw(3, 1, 0, 0);
    m_CommandBuffer.endRendering();

    TransitionImageLayout(
        m_VulkanSwapChain.m_Images,
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
