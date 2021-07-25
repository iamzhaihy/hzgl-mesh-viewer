#include "Camera.hpp"

#include <glm/gtx/transform.hpp>

hzgl::Camera::Camera() : Camera(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 45.0f, 4.0f / 3.0f)
{
}

hzgl::Camera::Camera(glm::vec3 _lookfrom, glm::vec3 _lookat, glm::vec3 _vup, float _vfov, float _aspect_ratio)
    : position(_lookfrom), target(_lookat), vfov(_vfov), aspect_ratio(_aspect_ratio)
{
    auto theta = glm::radians(vfov);
    auto h = glm::tan(theta / 2);

    w = glm::normalize(position - target);
    u = glm::normalize(glm::cross(_vup, w));
    v = glm::cross(w, u);
}

glm::mat4 hzgl::Camera::GetViewMatrix()
{
    return glm::lookAt(position, target, glm::vec3(0, 1, 0));
}

glm::mat4 hzgl::Camera::GetProjMatrix()
{
    return glm::perspective(glm::radians(vfov), aspect_ratio, 0.1f, 100.0f);
}

void hzgl::Camera::Move(const glm::vec3 direction, float speed, float deltaTime = (1 / 60))
{
    position += deltaTime * speed * direction;
}