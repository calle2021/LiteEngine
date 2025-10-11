#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace LiteEngine
{
class Camera
{
public:
    virtual ~Camera() = default;
    virtual glm::mat4 GetView() const = 0;
    virtual glm::mat4 GetProjection() const = 0;
};

}