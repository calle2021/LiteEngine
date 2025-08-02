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
    std::pair<uint32_t, uint32_t> GetResolution() { return m_Resolution; };
    std::pair<uint32_t, uint32_t> GetPixelResolution() { return m_PixelResolution; };
    GLFWwindow *GetWindowHandle() { return m_GLFWWindow; };
private:
    GLFWwindow *m_GLFWWindow;
private:
    std::pair<uint32_t, uint32_t> m_Resolution;
    std::pair<uint32_t, uint32_t> m_PixelResolution;
    double time;
};