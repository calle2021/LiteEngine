#include "GLFWindow.h"

#include "pch.h"

void Window::Init()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize glfw");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_GLFWWindow = glfwCreateWindow(1280, 720, "GameSystem", nullptr, nullptr);
    if (!m_GLFWWindow)
        throw std::runtime_error("Failed to create glfw window");
    glfwMakeContextCurrent(m_GLFWWindow);
}

bool Window::Close()
{
    return glfwWindowShouldClose(m_GLFWWindow);
}

void Window::Update()
{
    glfwPollEvents();
}

double Window::GetDeltaTime()
{
    return glfwGetTime();
}

void Window::Destroy()
{
    glfwDestroyWindow(m_GLFWWindow);
    glfwTerminate();
}