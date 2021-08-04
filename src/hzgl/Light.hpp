#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace hzgl
{
    enum LightType
    {
        HZGL_SPOT_LIGHT,
        HZGL_POINT_LIGHT,
        HZGL_DIRECTIONAL_LIGHT
    };

    class Light
    {
    public:
        bool isEnabled;
        LightType type;

        // for local lights: position
        // for directional light: direction
        glm::vec3 position;

        // color intensity
        glm::vec3 color;
        glm::vec3 ambient;

        // for spot light
        glm::vec3 coneDirection;
        float spotExponent;
        float spotCosCutoff;

        // for local lights
        float constantAttenuation;
        float linearAttenuation;
        float quadraticAttenuation;

        Light(LightType t = HZGL_POINT_LIGHT, const glm::vec3 &pos = glm::vec3(0, 1, 0),
              const glm::vec3 &col = glm::vec3(1, 1, 1), const glm::vec3 &amb = glm::vec3(.1, .1, .1));
    };

    void SetupLight(GLuint program, const Light &light, std::string uName = "uLight");
    void SetupLightInArray(GLuint program, const Light &light, std::string uArrayName = "uLights", int index = 0);
} // namespace hzgl
