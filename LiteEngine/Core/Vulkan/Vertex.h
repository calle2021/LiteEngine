#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace LiteVulkan {
struct Vertex
{
    glm::vec3 pos {};
    glm::vec3 color {};
    glm::vec2 tex_coord {};
    static vk::VertexInputBindingDescription GetBindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 3> GetAttributeDescriptions();

    constexpr bool operator==(const Vertex&) const noexcept = default;
};
}

namespace std {
template <> struct hash<LiteVulkan::Vertex>
{
    size_t operator()(LiteVulkan::Vertex const& vertex) const noexcept
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1)
            ^ (hash<glm::vec2>()(vertex.tex_coord) << 1);
    }
};
}