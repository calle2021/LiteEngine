#include "Window.h"
#include "Core/Logging/Logger.h"


constexpr uint32_t WIDTH = 1280;
constexpr uint32_t HEIGHT = 720;

namespace LiteEngine {
Window::Window()
    : m_Width(WIDTH)
    , m_Height(HEIGHT)
{
    if (!glfwInit()) {
        CORE_LOG_ERROR("Failed to initialize glfw.");
        throw std::runtime_error("Failed to initialize glfw.");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_GLFWindow = glfwCreateWindow(WIDTH, HEIGHT, "LiteEngine", nullptr, nullptr);

    if (!m_GLFWindow) {
        CORE_LOG_ERROR("Failed to create glfw window.");
        throw std::runtime_error("Failed to create glfw window.");
    }

    CORE_LOG_INFO("GLFW window created.");

    glfwMakeContextCurrent(m_GLFWindow);
    time = glfwGetTime();
    glfwSetWindowUserPointer(m_GLFWindow, this);
    glfwSetFramebufferSizeCallback(m_GLFWindow, FrameBufferResizeCallback);
}

void Window::FrameBufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto container = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    container->m_FrameBufferResized = true;
    container->m_Width = width;
    container->m_Height = height;
}

bool Window::Close()
{
    return glfwWindowShouldClose(m_GLFWindow);
}

void Window::Update()
{
    glfwPollEvents();
}

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