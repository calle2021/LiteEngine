#pragma once
#include <GLFW/glfw3.h>

namespace GameSystem
{
class Window
{
  public:
    static void Init();
    static void Update();
    static void Destroy();
    static bool Close();
    static double GetDeltaTime();

  private:
    static GLFWwindow *window;
};
} // namespace GameSystem