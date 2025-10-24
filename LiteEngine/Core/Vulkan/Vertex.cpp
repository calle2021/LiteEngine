#include "Vertex.h"

namespace LiteVulkan {

vk::VertexInputBindingDescription Vertex::GetBindingDescription()
{
    return { 0, sizeof(Vertex), vk::VertexInputRate::eVertex };
}

std::array<vk::VertexInputAttributeDescription, 3> Vertex::GetAttributeDescriptions()
{
    return std::to_array({ vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat,
                                                               offsetof(Vertex, pos)),
                           vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat,
                                                               offsetof(Vertex, color)),
                           vk::VertexInputAttributeDescription(2, 0, vk::Format::eR32G32Sfloat,
                                                               offsetof(Vertex, tex_coord)) });
}

}