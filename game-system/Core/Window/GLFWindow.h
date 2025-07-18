#pragma once
#include <GLFW/glfw3.h>

class GLFWindow
{
  public:
    void Init();
    void Update();
    void Destroy();
    bool Close();
    double GetDeltaTime();
    GLFWwindow *GetWindowHandle() { return m_GLFWWindow; };
  private:
    GLFWwindow *m_GLFWWindow;
};