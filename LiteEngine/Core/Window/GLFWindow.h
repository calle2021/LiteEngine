#pragma once
#include <GLFW/glfw3.h>
#include <utility>

class GLFWindow
{
public:
    void Init();
    void Update();
    void Destroy();
    bool Close();
    double GetDeltaTime();
    std::pair<uint32_t, uint32_t> GetResolution() const { return m_Resolution; };
    GLFWwindow* GetWindowHandle() const { return m_GLFWindow; };
    bool HasResized() { return m_FrameBufferResized; };
    void ResizeHandled() { m_FrameBufferResized = false; };
private:
    GLFWwindow* m_GLFWindow;
private:
    static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
    void SetResolution();
    std::pair<uint32_t, uint32_t> m_Resolution;
    double time;
    bool m_FrameBufferResized = false;
};