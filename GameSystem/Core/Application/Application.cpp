#include "pch.h"
#include "Application.h"
#include <GLFW/glfw3.h>

#if !(defined(WINDOWS) || defined(LINUX))
#error "Unsupported platform."
#endif

namespace GameSystem {
int Application::Launch(std::unique_ptr<GameApp> game) {
    Logger::Init();

    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1280, 720, "GameSystem", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    while (true) {
        if (glfwWindowShouldClose(window))
            break;
        glfwPollEvents();
        double dt = glfwGetTime();
        game->Update();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
}  // namespace GameSystem