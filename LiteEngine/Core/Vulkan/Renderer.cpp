#include "Renderer.h"
#include "Core/Logging/Logger.h"
#include "Config.h"

namespace LiteVulkan {
Renderer::Renderer(Buffers& buf, SwapChain& swap, Device& dev,
                   Pipeline& pipe, Assets& assets, LiteEngine::Window& win)
    : m_Buffers(buf)
    , m_SwapChain(swap)
    , m_Device(dev)
    , m_Pipeline(pipe)
    , m_AssetsRef(assets)
    , m_Window(win) {}

void Renderer::DrawFrame()
{
    while (vk::Result::eTimeout == m_Device.m_Device.waitForFences(*m_Fences[m_CurrentFrame], vk::True, UINT64_MAX))
    {
        CORE_LOG_INFO("Timeout!");
    }


    if(m_Window.HasResized())
    {
        RecreateSwapChain();
        return;
    }

    auto [result, imageIndex] = m_SwapChain.m_SwapChain.acquireNextImage(UINT64_MAX, *m_PresentSemaphores[m_CurrentFrame], nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        RecreateSwapChain();
        return;
    }

    m_Buffers.UpdateUniformBuffer(m_CurrentFrame);

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
            RecreateSwapChain();
            break;
        default:
            CORE_LOG_ERROR("Failed to present image");
            throw std::runtime_error("Failed to present image");
            break;
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % FRAMES_IN_FLIGHT;
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
        .commandBufferCount = FRAMES_IN_FLIGHT,
    };
    m_CommandBuffers = vk::raii::CommandBuffers(m_Device.m_Device, allocInfo);
    CORE_LOG_INFO("Commandbuffer created.");
}

void Renderer::CreateSyncObjects()
{
    m_PresentSemaphores.clear();
    m_RenderSemaphores.clear();
    m_Fences.clear();

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        m_PresentSemaphores.emplace_back(m_Device.m_Device, vk::SemaphoreCreateInfo());
        m_RenderSemaphores.emplace_back(m_Device.m_Device, vk::SemaphoreCreateInfo());
        m_Fences.emplace_back(m_Device.m_Device, vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled });
    }

    CORE_LOG_INFO("Using {} frames in flight in flight", FRAMES_IN_FLIGHT);
    CORE_LOG_INFO("Sync objects created");
}

void Renderer::RecordCommandBuffer(uint32_t imageIndex)
{
    m_CommandBuffers[m_CurrentFrame].begin({});
    TransitionImageLayout(
        m_SwapChain.m_Images[imageIndex],
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eTopOfPipe,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    TransitionImageLayout(
        m_AssetsRef.m_ColorImage,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eTopOfPipe,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    TransitionImageLayout(
        m_AssetsRef.m_DepthImage,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
        vk::PipelineStageFlagBits2::eTopOfPipe,
        vk::PipelineStageFlagBits2::eEarlyFragmentTests,
        vk::ImageAspectFlagBits::eDepth
    );

    vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
    vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

    vk::RenderingAttachmentInfo colorAttachment = {};
    colorAttachment.imageView = m_AssetsRef.m_ColorImageView;
    colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.resolveMode = vk::ResolveModeFlagBits::eAverage;
    colorAttachment.resolveImageView = m_SwapChain.m_ImageViews[imageIndex];
    colorAttachment.resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.clearValue = clearColor;

    vk::RenderingAttachmentInfo depthAttachmentInfo = {
        .imageView = m_AssetsRef.m_DepthBufferView,
        .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .clearValue = clearDepth
    };
    vk::RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = m_SwapChain.m_Extent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
        .pDepthAttachment = &depthAttachmentInfo
    };

    m_CommandBuffers[m_CurrentFrame].beginRendering(renderingInfo);
    m_CommandBuffers[m_CurrentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, *m_Pipeline.m_Pipeline);
    m_CommandBuffers[m_CurrentFrame].setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_SwapChain.m_Extent.width), static_cast<float>(m_SwapChain.m_Extent.height), 0.0f, 1.0f));
    m_CommandBuffers[m_CurrentFrame].setScissor(0, vk::Rect2D(vk::Offset2D( 0, 0 ), m_SwapChain.m_Extent));
    m_CommandBuffers[m_CurrentFrame].bindVertexBuffers(0, *m_Buffers.m_Buffers, {0});
    m_CommandBuffers[m_CurrentFrame].bindIndexBuffer( *m_Buffers.m_IndexBuffer, 0, vk::IndexTypeValue<decltype(m_Buffers.m_Indices)::value_type>::value );
    m_CommandBuffers[m_CurrentFrame].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Pipeline.m_PipelineLayout, 0, *m_Pipeline.m_DescriptorSets[m_CurrentFrame], nullptr);
    m_CommandBuffers[m_CurrentFrame].drawIndexed(m_Buffers.m_Indices.size(), 1, 0, 0, 0);
    m_CommandBuffers[m_CurrentFrame].endRendering();

    TransitionImageLayout(
        m_SwapChain.m_Images[imageIndex],
        vk::ImageLayout::eColorAttachmentOptimal,
        vk::ImageLayout::ePresentSrcKHR,
        vk::AccessFlagBits2::eColorAttachmentWrite,
        {},
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::PipelineStageFlagBits2::eBottomOfPipe,
        vk::ImageAspectFlagBits::eColor
    );
    m_CommandBuffers[m_CurrentFrame].end();
}
template <typename T>
void Renderer::TransitionImageLayout(
    const T& image,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    vk::AccessFlags2 srcAccessMask,
    vk::AccessFlags2 dstAccessMask,
    vk::PipelineStageFlags2 srcStageMask,
    vk::PipelineStageFlags2 dstStageMask,
    vk::ImageAspectFlags aspect_mask)
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
        .image = image,
        .subresourceRange = {
            .aspectMask = aspect_mask,
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

std::unique_ptr<vk::raii::CommandBuffer> Renderer::BeginSingleTimeCommands() {
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = m_CommandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    std::unique_ptr<vk::raii::CommandBuffer> cmdbuf = std::make_unique<vk::raii::CommandBuffer>(std::move(vk::raii::CommandBuffers(m_Device.m_Device, allocInfo).front()));
    vk::CommandBufferBeginInfo beginfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    cmdbuf->begin(beginfo);
    return cmdbuf;
}

void Renderer::EndSingleTimeCommands(vk::raii::CommandBuffer& cmdbuf) {
    cmdbuf.end();
    vk::SubmitInfo subinfo { .commandBufferCount = 1, .pCommandBuffers = &*cmdbuf };
    m_Device.m_Queue.submit(subinfo, nullptr);
    m_Device.m_Queue.waitIdle();
}

void Renderer::RecreateSwapChain()
{
    CORE_LOG_INFO("Recreate swap chain");
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_Window.GetWindowHandle(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_Window.GetWindowHandle(), &width, &height);
        glfwWaitEvents();
    }
    m_Device.m_Device.waitIdle();
    m_SwapChain.m_ImageViews.clear();
    m_SwapChain.m_SwapChain = nullptr;
    m_SwapChain.CreateSwapChain();
    m_SwapChain.CreateImageViews();
    m_AssetsRef.CreateColorResources();
    m_AssetsRef.CreateDepthResources();
    m_Window.ResizeHandled();
}

}