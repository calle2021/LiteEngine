#include "GraphicsPipeline.h"
#include "pch.h"
#include "vulkan-utils.h"

GraphicsPipeline::GraphicsPipeline(){};
GraphicsPipeline::~GraphicsPipeline(){};
void GraphicsPipeline::LoadShaders()
{
    auto vertShaderCode = GameSystem::VulkanUtils::readFile("assets/shaders/vert.spv");
    auto fragShaderCode = GameSystem::VulkanUtils::readFile("assets/shaders/frag.spv");
}
