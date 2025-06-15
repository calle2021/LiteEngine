#include "window.h"
#include "pch.h"

namespace GameSystem
{
GLFWwindow *Window::window;
void Window::Init()
{
    if (!glfwInit())
        throw std::runtime_error("Faield to initialize glfw");
    window = glfwCreateWindow(1280, 720, "GameSystem", nullptr, nullptr);
    if (!window)
        throw std::runtime_error("Faield to create glfw window");
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