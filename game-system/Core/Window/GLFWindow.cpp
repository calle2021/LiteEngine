#include "GLFWindow.h"

#include "pch.h"

void GLFWindow::Init()
{
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize glfw");
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_GLFWWindow = glfwCreateWindow(1280, 720, "GameSystem", nullptr, nullptr);

    if (!m_GLFWWindow) {
        throw std::runtime_error("Failed to create glfw window");
    }
    glfwMakeContextCurrent(m_GLFWWindow);
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
    return glfwGetTime();
}

void GLFWindow::Destroy()
{
    glfwDestroyWindow(m_GLFWWindow);
    glfwTerminate();
}