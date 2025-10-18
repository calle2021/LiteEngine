#include "Device.h"
#include "Context.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "Core/Logging/Logger.h"
#include <set>

const std::vector<const char*> rExtensions  = {
    vk::KHRSwapchainExtensionName,
    vk::KHRSpirv14ExtensionName,
    vk::KHRSynchronization2ExtensionName,
    vk::KHRCreateRenderpass2ExtensionName
};

namespace LiteVulkan {
void Device::PickPhysicalDevice(const vk::raii::Instance& instance)
{
    auto devices = instance.enumeratePhysicalDevices();

    if (!devices.size()) {
        throw std::runtime_error("No device with vulkan support found!");
    }

    for (const auto& dev: devices) {
        if(!DeviceSuitable(dev)) continue;
        m_PhysicalDevice = dev;
        break;
    }
    if (m_PhysicalDevice == nullptr) {
        throw std::runtime_error("Failed to find suitable device!");
    }
    CORE_LOG_INFO("Found suitable device!");
    m_MsaaSamples = GetMaxUsableSampleCount();
}

bool Device::DeviceSuitable(const vk::raii::PhysicalDevice& device) const
{
    if (device.getProperties().apiVersion < VK_API_VERSION_1_3) return false;

    auto qFamilies = device.getQueueFamilyProperties();
    bool hasGraphics = false;
    for (const auto& qFamily: qFamilies)
    {
        if(qFamily.queueFlags & vk::QueueFlagBits::eGraphics)
        {
            hasGraphics = true;
            break;
        }
    }
    if (!hasGraphics) return false;

    auto aExtensions = device.enumerateDeviceExtensionProperties();
    std::set<std::string> rExtensionsSet(rExtensions.begin(), rExtensions.end());
    for (const auto& ext: aExtensions) {
        rExtensionsSet.erase(ext.extensionName);
    }
    if (!rExtensionsSet.empty()) return false;

    auto dFeatures = device.getFeatures2<
        vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

    const auto& pDevFeatures = dFeatures.get<vk::PhysicalDeviceFeatures2>().features;
    const auto& vk13Features = dFeatures.get<vk::PhysicalDeviceVulkan13Features>();
    const auto& extDynFeatures = dFeatures.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool hasFeatures = pDevFeatures.samplerAnisotropy && vk13Features.dynamicRendering && extDynFeatures.extendedDynamicState;
    if (!hasFeatures) return false;
    return true;
}

void Device::CreateLogicalDevice(const vk::raii::SurfaceKHR& surface)
{
    std::vector<vk::QueueFamilyProperties> qFamilies = m_PhysicalDevice.getQueueFamilyProperties();

    uint32_t qIndex = 0;
    for (const auto& qFamily : qFamilies)
    {
        bool gSupport = (qFamily.queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlags{};
        bool pSupport = m_PhysicalDevice.getSurfaceSupportKHR(qIndex , *surface);
        if (gSupport && pSupport) {
            m_QueueIndex = qIndex;
            break;
        }
        qIndex++;
    }

    if (!m_QueueIndex.has_value()) {
        throw std::runtime_error("Failed to find suitable queue family!");
    }

    vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features, vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT> fChain = {
        {.features = { .sampleRateShading = true, .samplerAnisotropy = true } },
        {.synchronization2 = true, .dynamicRendering = true },
        {.extendedDynamicState = true }
    };

    float qPrio = 0.0f;
    vk::DeviceQueueCreateInfo qInfo = {};
    qInfo.queueFamilyIndex = m_QueueIndex.value();
    qInfo.queueCount = 1;
    qInfo.pQueuePriorities = &qPrio;

    vk::DeviceCreateInfo dInfo = {};
    dInfo.pNext = &fChain.get<vk::PhysicalDeviceFeatures2>(),
    dInfo.queueCreateInfoCount = 1;
    dInfo.pQueueCreateInfos = &qInfo;
    dInfo.enabledExtensionCount = static_cast<uint32_t>(rExtensions.size());
    dInfo.ppEnabledExtensionNames = rExtensions.data();

    m_Device = vk::raii::Device(m_PhysicalDevice, dInfo);
    m_Queue = vk::raii::Queue(m_Device, m_QueueIndex.value(), 0);
}

vk::SampleCountFlagBits Device::GetMaxUsableSampleCount()
{
    vk::PhysicalDeviceProperties pdProps = m_PhysicalDevice.getProperties();
    vk::SampleCountFlags counts = pdProps.limits.framebufferColorSampleCounts & pdProps.limits.framebufferDepthSampleCounts;
    if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
    if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
    if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
    if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
    if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
    if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }
    return vk::SampleCountFlagBits::e1;
}
}