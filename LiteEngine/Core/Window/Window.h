#pragma once
#include <GLFW/glfw3.h>
#include <utility>

namespace LiteEngine {
class Window
{
public:
    Window(uint32_t width, uint32_t height);
    void Update();
    void Destroy();
    bool Close();
    double GetDeltaTime();
    uint32_t GetWidth() const { return m_Width; };
    uint32_t GetHeight() const { return m_Height; };
    float GetAspect() const { return static_cast<float>(m_Width) / static_cast<float>(m_Height); };
    GLFWwindow* GetWindowHandle() const { return m_GLFWindow; };
    bool HasResized() { return m_FrameBufferResized; };
    void ResizeHandled() { m_FrameBufferResized = false; };

private:
    GLFWwindow* m_GLFWindow;

private:
    uint32_t m_Width;
    uint32_t m_Height;
    double time;
    bool m_FrameBufferResized = false;

private:
    static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);
};
}