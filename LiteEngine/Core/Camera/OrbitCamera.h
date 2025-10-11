#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"

namespace LiteEngine
{

class OrbitCamera : public Camera
{
public:
    OrbitCamera(float aspect);
    glm::mat4 GetView() const override;
    glm::mat4 GetProjection() const override;
private:
    glm::vec3 m_Pos;
    glm::vec3 m_Target;
    glm::vec3 m_Up;
    float m_FoV;
    float m_Aspect;
    float m_Near;
    float m_Far;
};
}