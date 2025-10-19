#include "Renderer.h"
#include "Core/Logging/Logger.h"
#include <iostream>

const std::vector validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

constexpr uint32_t nFramesInFlight = 2;

#ifdef NDEBUG
constexpr bool enable_validation_layers = false;
#else
constexpr bool enable_validation_layers = true;
#endif
namespace LiteVulkan {

Renderer::Renderer(Window& window, const Camera& camera) : m_Window(window) , m_Camera(camera) {}

void Renderer::Init()
{
    m_Instance = CreateInstance();
    m_DebugMessenger = SetupDebugMessenger();
    m_Surface = CreateSurface();

    m_Device = std::make_unique<Device>();
    m_Device->PickPhysicalDevice(m_Instance);
    m_Device->CreateLogicalDevice(m_Surface);

    m_SwapChain = std::make_unique<SwapChain>(*m_Device);
    m_SwapChain->CreateSwapChain(m_Window.GetWidth(), m_Window.GetHeight(), m_Surface);
    m_SwapChain->CreateImageViews();

    m_Assets = std::make_unique<Assets>(*this, *m_Device);
    m_Pipeline = std::make_unique<Pipeline>(*m_Device);
    m_Pipeline->CreateDescriptorLayout();
    m_Pipeline->CreatePipeline(m_SwapChain->GetImageFormat(), m_Assets->GetDepthFormat());
    m_CommandPool = CreateCommandPool();

    m_Assets->CreateColorResources(m_SwapChain->GetExtentWidth(), m_SwapChain->GetExtentHeight(), m_SwapChain->GetImageFormat());
    m_Assets->CreateDepthResources(m_SwapChain->GetExtentWidth(), m_SwapChain->GetExtentHeight());
    m_Assets->CreateTexture();
    m_Assets->CreateTextureImageView();
    m_Assets->CreateTextureSampler();
    m_Assets->LoadModel();

    m_Buffers = std::make_unique<Buffers>(*m_Device, m_Camera);

    m_Buffers->BufferModels(m_Assets->GetShapes(), m_Assets->GetAttributes());
    m_Buffers->CreateVertexBuffer(m_CommandPool);
    m_Buffers->CreateIndexBuffer(m_CommandPool);
    m_Buffers->CreateUniformBuffers(nFramesInFlight);
    m_Pipeline->CreateDescriptorPool(nFramesInFlight);
    m_Pipeline->CreateDescriptorSets(nFramesInFlight, m_Assets->GetSampler(), m_Assets->GetTextureImageView(), m_Buffers->GetUniformBuffers());
    m_CommandBuffers = CreateCommandBuffers();
    CreateSyncObjects();
}

void Renderer::DrawFrame()
{
    while (vk::Result::eTimeout == m_Device->GetDevice().waitForFences(*m_Fences[m_CurrentFrame], vk::True, UINT64_MAX))
    {
        CORE_LOG_INFO("Timeout!");
    }


    if(m_Window.HasResized())
    {
        RecreateSwapChain();
        return;
    }

    auto [result, imageIndex] = m_SwapChain->GetSwapChain().acquireNextImage(UINT64_MAX, *m_PresentSemaphores[m_CurrentFrame], nullptr);

    if (result == vk::Result::eErrorOutOfDateKHR)
    {
        RecreateSwapChain();
        return;
    }

    m_Buffers->UpdateUniformBuffer(m_CurrentFrame);

    m_Device->GetDevice().resetFences(*m_Fences[m_CurrentFrame]);
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
    m_Device->GetQueue().submit(submitInfo, *m_Fences[m_CurrentFrame]);

    const vk::PresentInfoKHR presentInfoKHR {
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*m_RenderSemaphores[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &*m_SwapChain->GetSwapChain(),
        .pImageIndices = &imageIndex
    };

    result = m_Device->GetQueue().presentKHR(presentInfoKHR);
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

    m_CurrentFrame = (m_CurrentFrame + 1) % nFramesInFlight;
}

void Renderer::Shutdown()
{
    m_Device->GetDevice().waitIdle();
    m_CommandBuffers.clear();
    m_CommandPool = nullptr;
    m_PresentSemaphores.clear();
    m_RenderSemaphores.clear();
    m_Fences.clear();
    m_Pipeline.reset();
    m_Buffers.reset();
    m_SwapChain.reset();
    m_Assets.reset();
    m_Device.reset();
    m_Surface = nullptr;
    m_DebugMessenger = nullptr;
    m_Instance = nullptr;
}

[[nodiscard]] vk::raii::Instance Renderer::CreateInstance()
{
    constexpr vk::ApplicationInfo AppInfo {
        .pApplicationName = "Sample",
        .applicationVersion = VK_MAKE_VERSION( 1, 0, 0 ),
        .pEngineName = "Engine",
        .engineVersion = VK_MAKE_VERSION( 1, 0, 0 ),
        .apiVersion = vk::ApiVersion14
    };

    std::vector<char const*> requiredLayers;
    if (enable_validation_layers) {
        requiredLayers.assign(validationLayers.begin(), validationLayers.end());
    }

    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    if (enable_validation_layers) {
        requiredExtensions.push_back(vk::EXTDebugUtilsExtensionName);
    }

    vk::InstanceCreateInfo CreateInfo {
        .pApplicationInfo = &AppInfo,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data(),
    };
    return vk::raii::Instance(m_Context, CreateInfo);
}

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) {
    std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
    return vk::False;
}

[[nodiscard]] vk::raii::DebugUtilsMessengerEXT Renderer::SetupDebugMessenger()
{
    if (!enable_validation_layers) {
        return nullptr;
    }
    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
    vk::DebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfoEXT {
        .messageSeverity = severityFlags,
        .messageType = messageTypeFlags,
        .pfnUserCallback = &debugCallback
    };

    return m_Instance.createDebugUtilsMessengerEXT(debugUtilsMessengerCreateInfoEXT);
}

[[nodiscard]] vk::raii::SurfaceKHR Renderer::CreateSurface()
{
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(*m_Instance, m_Window.GetWindowHandle(), nullptr, &surface) != 0) {
        throw std::runtime_error("Failed to create window surface.");
    }
    return vk::raii::SurfaceKHR(m_Instance, surface);
}

vk::raii::CommandPool Renderer::CreateCommandPool()
{
    vk::CommandPoolCreateInfo poolInfo {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = m_Device->GetQueueIndex(),
    };
    return vk::raii::CommandPool(m_Device->GetDevice(), poolInfo);
}

std::vector<vk::raii::CommandBuffer> Renderer::CreateCommandBuffers()
{
    m_CommandBuffers.clear();
    vk::CommandBufferAllocateInfo allocInfo {
        .commandPool = m_CommandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = nFramesInFlight,
    };
    return vk::raii::CommandBuffers(m_Device->GetDevice(), allocInfo);
}

void Renderer::CreateSyncObjects()
{
    m_PresentSemaphores.clear();
    m_RenderSemaphores.clear();
    m_Fences.clear();

    for (size_t i = 0; i < nFramesInFlight; i++)
    {
        m_PresentSemaphores.emplace_back(m_Device->GetDevice(), vk::SemaphoreCreateInfo());
        m_RenderSemaphores.emplace_back(m_Device->GetDevice(), vk::SemaphoreCreateInfo());
        m_Fences.emplace_back(m_Device->GetDevice(), vk::FenceCreateInfo { .flags = vk::FenceCreateFlagBits::eSignaled });
    }
}

void Renderer::RecreateSwapChain()
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_Window.GetWindowHandle(), &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_Window.GetWindowHandle(), &width, &height);
        glfwWaitEvents();
    }
    m_Device->GetDevice().waitIdle();
    m_SwapChain->Reset();
    m_SwapChain->CreateSwapChain(m_Window.GetWidth(), m_Window.GetHeight(), m_Surface);
    m_SwapChain->CreateImageViews();
    m_Assets->CreateColorResources(m_SwapChain->GetExtentWidth(), m_SwapChain->GetExtentHeight(), m_SwapChain->GetImageFormat());
    m_Assets->CreateDepthResources(m_SwapChain->GetExtentWidth(), m_SwapChain->GetExtentHeight());
    m_Window.ResizeHandled();
}

void Renderer::RecordCommandBuffer(uint32_t imageIndex)
{
    m_CommandBuffers[m_CurrentFrame].begin({});
    TransitionImageLayout(
        m_SwapChain->m_Images[imageIndex],
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eTopOfPipe,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    TransitionImageLayout(
        m_Assets->m_ColorImage,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal,
        {},
        vk::AccessFlagBits2::eColorAttachmentWrite,
        vk::PipelineStageFlagBits2::eTopOfPipe,
        vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        vk::ImageAspectFlagBits::eColor
    );

    TransitionImageLayout(
        m_Assets->m_DepthImage,
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
    colorAttachment.imageView = m_Assets->m_ColorImageView;
    colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.resolveMode = vk::ResolveModeFlagBits::eAverage;
    colorAttachment.resolveImageView = m_SwapChain->m_ImageViews[imageIndex];
    colorAttachment.resolveImageLayout = vk::ImageLayout::eColorAttachmentOptimal;
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.clearValue = clearColor;

    vk::RenderingAttachmentInfo depthAttachmentInfo = {
        .imageView = m_Assets->m_DepthBufferView,
        .imageLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eDontCare,
        .clearValue = clearDepth
    };
    vk::RenderingInfo renderingInfo = {
        .renderArea = { .offset = { 0, 0 }, .extent = m_SwapChain->m_Extent },
        .layerCount = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment,
        .pDepthAttachment = &depthAttachmentInfo
    };

    m_CommandBuffers[m_CurrentFrame].beginRendering(renderingInfo);
    m_CommandBuffers[m_CurrentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, *m_Pipeline->m_Pipeline);
    m_CommandBuffers[m_CurrentFrame].setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(m_SwapChain->m_Extent.width), static_cast<float>(m_SwapChain->m_Extent.height), 0.0f, 1.0f));
    m_CommandBuffers[m_CurrentFrame].setScissor(0, vk::Rect2D(vk::Offset2D( 0, 0 ), m_SwapChain->m_Extent));
    m_CommandBuffers[m_CurrentFrame].bindVertexBuffers(0, *m_Buffers->m_Buffers, {0});
    m_CommandBuffers[m_CurrentFrame].bindIndexBuffer( *m_Buffers->m_IndexBuffer, 0, vk::IndexTypeValue<decltype(m_Buffers->m_Indices)::value_type>::value );
    m_CommandBuffers[m_CurrentFrame].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_Pipeline->m_PipelineLayout, 0, *m_Pipeline->m_DescriptorSets[m_CurrentFrame], nullptr);
    m_CommandBuffers[m_CurrentFrame].drawIndexed(m_Buffers->m_Indices.size(), 1, 0, 0, 0);
    m_CommandBuffers[m_CurrentFrame].endRendering();

    TransitionImageLayout(
        m_SwapChain->m_Images[imageIndex],
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

std::unique_ptr<vk::raii::CommandBuffer> Renderer::BeginSingleTimeCommands() const {
    vk::CommandBufferAllocateInfo allocInfo{ .commandPool = m_CommandPool, .level = vk::CommandBufferLevel::ePrimary, .commandBufferCount = 1 };
    std::unique_ptr<vk::raii::CommandBuffer> cmdbuf = std::make_unique<vk::raii::CommandBuffer>(std::move(vk::raii::CommandBuffers(m_Device->GetDevice(), allocInfo).front()));
    vk::CommandBufferBeginInfo beginfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
    cmdbuf->begin(beginfo);
    return cmdbuf;
}

void Renderer::EndSingleTimeCommands(vk::raii::CommandBuffer& cmdbuf) const {
    cmdbuf.end();
    vk::SubmitInfo subinfo { .commandBufferCount = 1, .pCommandBuffers = &*cmdbuf };
    m_Device->GetQueue().submit(subinfo, nullptr);
    m_Device->GetQueue().waitIdle();
}
}