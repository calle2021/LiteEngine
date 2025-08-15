#include "Renderer.h"
#include "Core/Logging/Logger.h"

namespace LiteVulkan {

const uint32_t MaxFramesInFlight = 2;

Renderer::Renderer(VertexBuffer& vertexBuffer, SwapChain& swapChain, Device& device,
                   GraphicsPipeline& graphicsPipeline, GLFWindow& window)
    : m_VertexBuffer(vertexBuffer)
    , m_SwapChain(swapChain)
    , m_Device(device)
    , m_GraphicsPipeline(graphicsPipeline)
    , m_Window(window) {}

void Renderer::DrawFrame()
{
    while (vk::Result::eTimeout == m_Device.m_Device.waitForFences(*m_Fences[m_CurrentFrame], vk::True, UINT64_MAX))
    {
        CORE_LOG_INFO("Timeout!");
    }


    if(m_Window.HasResized())
    {
        m_SwapChain.RecreateSwapChain();
        return;
    }

    auto [result, imageIndex] = m_SwapChain.m_SwapChain.acquireNextImage(UINT64_MAX, *m_PresentSemaphores[m_CurrentFrame], nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        m_SwapChain.RecreateSwapChain();
        return;
    }

    m_Device.m_Device.resetFences(*m_Fences[m_CurrentFrame]);
    m_CommandBuffers[m_CurrentFrame].reset();
    RecordCommandBuffer(imageIndex);

    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo submitInfo {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*m_PresentSemaphores[m_CurrentFrame],
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &*m_CommandBuffers[m_CurrentFrame],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*m_RenderSemaphores[imageIndex]
    };
    m_Device.m_Queue.submit(submitInfo, *m_Fences[m_CurrentFrame]);

    const vk::PresentInfoKHR presentInfoKHR {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*m_RenderSemaphores[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &*m_SwapChain.m_SwapChain,
        .pImageIndices = &imageIndex
    };

    result = m_Device.m_Queue.presentKHR(presentInfoKHR);

    result = m_Window.HasResized() ? vk::Result::eErrorOutOfDateKHR : result;

    switch (result)
    {
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
            CORE_LOG_INFO("what the helly");
            break;
        case vk::Result::eErrorOutOfDateKHR:
            CORE_LOG_INFO("eErrorOutOfDateKHR");
            m_SwapChain.RecreateSwapChain();
            break;
        default:
            CORE_LOG_ERROR("Failed to present image");
            throw std::runtime_error("Failed to present image");
            break;
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % MaxFramesInFlight;
}

void Renderer::CreateCommandPool()
{
    vk::CommandPoolCreateInfo poolInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = m_Device.m_QueueIndex.value()
    };
    m_CommandPool = vk::raii::CommandPool(m_Device.m_Device, poolInfo);
    CORE_LOG_INFO("Commandpool created with queue family index {}", poolInfo.queueFamilyIndex);
}

void Renderer::CreateCommandBuffers()
{
    m_CommandBuffers.clear();
    vk::CommandBufferAllocateInfo allocInfo {
        .commandPool = m_CommandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = MaxFramesInFlight,
    };
    m_CommandBuffers = vk::raii::CommandBuffers(m_Device.m_Device, allocInfo);
    CORE_LOG_INFO("Commandbuffer created.");
}

void Renderer::CreateSyncObjects()
{
    m_PresentSemaphores.clear();
    m_RenderSemaphores.clear();
    m_Fences.clear();

    for (size_t i = 0; i < MaxFramesInFlight; i++)
    {
        m_PresentSemaphores.emplace_back(m_Device.m_Device, vk::SemaphoreCreateInfo());
        m_RenderSemaphores.emplace_back(m_Device.m_Device, vk::SemaphoreCreateInfo());
        m_Fences.emplace_back(m_Device.m_Device, vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled });
    }

    CORE_LOG_INFO("Using {} frames in flight in flight", MaxFramesInFlight);
    CORE_LOG_INFO("Sync objects created");
}

void Renderer::RecordCommandBuffer(uint32_t imageIndex)
{
    m_CommandBuffers[m_CurrentFrame].begin({});
    TransitionImageLayout(
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
        .imageView = m_SwapChain.m_ImageViews[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = clearColor
    };
    vk::RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = m_SwapChain.m_Extent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentInfo
    };

    m_CommandBuffers[m_CurrentFrame].beginRendering(renderingInfo);
    m_CommandBuffers[m_CurrentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, *m_GraphicsPipeline.m_GraphicsPipeline);
    m_CommandBuffers[m_CurrentFrame].setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_SwapChain.m_Extent.width), static_cast<float>(m_SwapChain.m_Extent.height), 0.0f, 1.0f));
    m_CommandBuffers[m_CurrentFrame].setScissor(0, vk::Rect2D(vk::Offset2D( 0, 0 ), m_SwapChain.m_Extent));
    m_CommandBuffers[m_CurrentFrame].bindVertexBuffers(0, *m_VertexBuffer.m_VertexBuffer, {0});
    m_CommandBuffers[m_CurrentFrame].draw(3, 1, 0, 0);
    m_CommandBuffers[m_CurrentFrame].endRendering();

    TransitionImageLayout(
        imageIndex,
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe
    );
    m_CommandBuffers[m_CurrentFrame].end();
}

void Renderer::TransitionImageLayout(
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
        .image = m_SwapChain.m_Images[imageIndex],
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
    m_CommandBuffers[m_CurrentFrame].pipelineBarrier2(dependencyInfo);
}
}