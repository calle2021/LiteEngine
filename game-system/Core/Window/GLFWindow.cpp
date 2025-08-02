#include "GLFWindow.h"
#include "Core/Logging/Logger.h"
#include "pch.h"

constexpr uint32_t WIDTH = 1280;
constexpr uint32_t HEIGHT = 720;

void GLFWindow::Init()
{
    if (!glfwInit()) {
        CORE_LOG_ERROR("Failed to initialize glfw.");
        throw std::runtime_error("Failed to initialize glfw.");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_GLFWWindow = glfwCreateWindow(WIDTH, HEIGHT, "GameSystem", nullptr, nullptr);

    if (!m_GLFWWindow) {
        CORE_LOG_ERROR("Failed to create glfw window.");
        throw std::runtime_error("Failed to create glfw window.");
    }

    glfwMakeContextCurrent(m_GLFWWindow);

    m_Resolution = std::make_pair(WIDTH, HEIGHT);
    int pixel_width, pixel_height;
    glfwGetFramebufferSize(m_GLFWWindow, &pixel_width, &pixel_height);
    m_PixelResolution = std::make_pair(pixel_width, pixel_height);
    CORE_LOG_INFO("GLFW window created ({}x{} {}x{}).",
        m_Resolution.first, m_Resolution.second, m_PixelResolution.first, m_PixelResolution.second);

    time = glfwGetTime();
}

bool GLFWindow::Close()
{
    return glfwWindowShouldClose(m_GLFWWindow);
}

void GLFWindow::Update()
{
    glfwPollEvents();
}

double GLFWindow::GetDeltaTime()
{
    double curr = glfwGetTime();
    double deltaTime = curr - time;
    time = curr;
    return deltaTime;
}

void GLFWindow::Destroy()
{
    glfwDestroyWindow(m_GLFWWindow);
    glfwTerminate();
}