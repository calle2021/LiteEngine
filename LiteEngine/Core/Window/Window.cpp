#include "Window.h"
#include "Core/Logging/Logger.h"

namespace LiteEngine {
Window::Window(uint32_t width, uint32_t height)
    : m_Width(width)
    , m_Height(height)
{
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw!");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_GLFWindow = glfwCreateWindow(m_Width, m_Height, "LiteEngine", nullptr, nullptr);

    if (!m_GLFWindow) {
        throw std::runtime_error("Failed to create glfw window!");
    }

    CORE_LOG_INFO("Window created");

    glfwMakeContextCurrent(m_GLFWindow);
    time = glfwGetTime();
    glfwSetWindowUserPointer(m_GLFWindow, this);
    glfwSetFramebufferSizeCallback(m_GLFWindow, FrameBufferResizeCallback);
}

void Window::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto container = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    container->m_FrameBufferResized = true;
    container->m_Width = width;
    container->m_Height = height;
}

bool Window::Close() { return glfwWindowShouldClose(m_GLFWindow); }

void Window::Update() { glfwPollEvents(); }

double Window::GetDeltaTime()
{
    double curr = glfwGetTime();
    double deltaTime = curr - time;
    time = curr;
    return deltaTime;
}

void Window::Destroy()
{
    glfwDestroyWindow(m_GLFWindow);
    glfwTerminate();
}
}