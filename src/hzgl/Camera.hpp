#pragma once

#include <glm/glm.hpp>

namespace hzgl
{
    class Camera
    {
    public:
        Camera();
        Camera(glm::vec3 _lookfrom, glm::vec3 _lookat, glm::vec3 _vup, float _vfov, float _aspect_ratio);

        glm::mat4 GetViewMatrix();
        glm::mat4 GetProjMatrix();

        void Move(const glm::vec3 direction, float speed, float deltaTime);

    public:
        glm::vec3 position;
        glm::vec3 target;
        glm::vec3 u, v, w;
        float vfov, aspect_ratio;
    };
} // namespace hzgl
