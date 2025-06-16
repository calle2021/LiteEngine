#include "window.h"
#include "pch.h"

namespace GameSystem
{
GLFWwindow *Window::window;
void Window::Init()
{
    if (!glfwInit())
        throw std::runtime_error("Failed to initialize glfw");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(1280, 720, "GameSystem", nullptr, nullptr);
    if (!window)
        throw std::runtime_error("Failed to create glfw window");
    glfwMakeContextCurrent(window);
}

bool Window::Close()
{
    return glfwWindowShouldClose(window);
}

void Window::Update()
{
    glfwPollEvents();
}

void Window::Destroy()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

double Window::GetDeltaTime()
{
    return glfwGetTime();
}
} // namespace GameSystem