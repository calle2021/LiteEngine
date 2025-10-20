#pragma once
#include <cstdint>
#include <optional>
#include <vulkan/vulkan_raii.hpp>

namespace LiteVulkan {
class Device
{
public:
    void PickPhysicalDevice(const vk::raii::Instance& instance);
    void CreateLogicalDevice(const vk::raii::SurfaceKHR& surface);
    const vk::raii::PhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; };
    const vk::raii::Device& GetDevice() const { return m_Device; };
    const vk::raii::Queue& GetQueue() const { return m_Queue; };
    vk::SampleCountFlagBits GetMsaaSamples() const { return m_MsaaSamples; };
    uint32_t GetQueueIndex() const { return m_QueueIndex.value(); };

private:
    bool DeviceSuitable(const vk::raii::PhysicalDevice& device) const;
    vk::SampleCountFlagBits GetMaxUsableSampleCount();

private:
    vk::raii::PhysicalDevice m_PhysicalDevice = nullptr;
    vk::raii::Device m_Device = nullptr;
    vk::raii::Queue m_Queue = nullptr;
    std::optional<uint32_t> m_QueueIndex;
    vk::SampleCountFlagBits m_MsaaSamples = vk::SampleCountFlagBits::e1;
};
}