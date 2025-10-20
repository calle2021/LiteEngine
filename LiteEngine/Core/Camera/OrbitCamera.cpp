#include "OrbitCamera.h"

namespace LiteEngine {
OrbitCamera::OrbitCamera(float aspect)
    : m_Aspect(aspect)
    , m_Pos(2.0f, 2.0f, 2.0f)
    , m_Target(0.0f, 0.0f, 0.0f)
    , m_Up(0.0f, 0.0f, 1.0f)
    , m_FoV(glm::radians(45.0f))
    , m_Near(0.1f)
    , m_Far(10.0f)
{
}

glm::mat4 OrbitCamera::GetView() const { return glm::lookAt(m_Pos, m_Target, m_Up); }

glm::mat4 OrbitCamera::GetProjection() const
{
    return glm::perspective(m_FoV, m_Aspect, m_Near, m_Far);
}

}