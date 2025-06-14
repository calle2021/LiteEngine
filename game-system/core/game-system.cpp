#include "game-system.h"
#include "pch.h"
#include <GLFW/glfw3.h>

#if !(defined(WINDOWS) || defined(LINUX))
#error "unsupported platform"
#endif

namespace GameSystem
{
int GameSystem::launch(std::unique_ptr<GameApp> game)
{
    Logger::init();
    CORE_INFO("Launching GameSystem");

    glfwInit();
    GLFWwindow *window = glfwCreateWindow(1280, 720, "GameSystem", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    while (true)
    {
        if (glfwWindowShouldClose(window))
            break;
        glfwPollEvents();
        double dt = glfwGetTime();
        game->update();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
} // namespace GameSystem