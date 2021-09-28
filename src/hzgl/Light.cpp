#include "Light.hpp"

hzgl::Light::Light(LightType t, const glm::vec3 &pos, const glm::vec3 &col, const glm::vec3 &amb)
    : type(t), position(pos), color(col), ambient(amb)
{
    isEnabled = true;

    // for spot light
    coneDirection = glm::vec3(0.0f, -1.0f, 0.0f);
    spotExponent = 1.0f;
    spotCosCutoff = 0.866f; // ~ cos(15 deg)

    // for local lights
    constantAttenuation = 1.0f;
    linearAttenuation = 0.35f;
    quadraticAttenuation = 0.44f;
}

static GLuint hzglLoc(GLuint program, const std::string& uName)
{
    return glGetUniformLocation(program, uName.c_str());
}

std::string hzgl::LightTypeName(LightType type)
{
    std::string lightType;

    switch (type)
    {
        case HZGL_SPOT_LIGHT:
            lightType = "Spot light";
            break;
        case HZGL_POINT_LIGHT:
            lightType = "Point light";
            break;
        case HZGL_DIRECTIONAL_LIGHT:
            lightType = "Directional light";
            break;
        default:
            lightType = "Unknown type";
            break;
    }

    return lightType;
}

void hzgl::SetupLight(GLuint program, const Light &light, std::string uName)
{
    // type of the light
    glUniform1i(hzglLoc(program, uName + ".isEnabled"), (light.isEnabled ? 1 : 0));
    glUniform1i(hzglLoc(program, uName + ".isLocal"), (light.type == HZGL_DIRECTIONAL_LIGHT ? 0 : 1));
    glUniform1i(hzglLoc(program, uName + ".isSpot"), (light.type == HZGL_SPOT_LIGHT ? 1 : 0));

    // position/direction
    glUniform3fv(hzglLoc(program, uName + ".position"), 1, &light.position[0]);

    // light intensities for different components
    glUniform3fv(hzglLoc(program, uName + ".color"), 1, &light.color[0]);
    glUniform3fv(hzglLoc(program, uName + ".ambient"), 1, &light.ambient[0]);

    // cone properties for spot light
    glUniform3fv(hzglLoc(program, uName + ".coneDirection"), 1, &light.coneDirection[0]);
    glUniform1f(hzglLoc(program, uName + ".spotExponent"), light.spotExponent);
    glUniform1f(hzglLoc(program, uName + ".spotCosCutoff"), light.spotCosCutoff);

    // attenuation factors for local light
    glUniform1f(hzglLoc(program, uName + ".constantAttenuation"), light.constantAttenuation);
    glUniform1f(hzglLoc(program, uName + ".linearAttenuation"), light.linearAttenuation);
    glUniform1f(hzglLoc(program, uName + ".quadraticAttenuation"), light.quadraticAttenuation);
}

void hzgl::SetupLightInArray(GLuint program, const Light &light, std::string uArrayName, int index)
{
    std::string uName = uArrayName + "[" + std::to_string(index) + "]";
    SetupLight(program, light, uName);
}