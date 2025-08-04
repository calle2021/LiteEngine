#pragma once
#include <memory>
#include "Core/Vulkan/VulkanContext.h"
#include "Core/Window/GLFWindow.h"
#include "iLiteBox.h"

namespace LiteEngine
{
class Application
{
public:
    int Launch(std::unique_ptr<iLiteBox> app);
private:
    GLFWindow m_Window;
    VulkanContext m_VulkanContext;
};
}