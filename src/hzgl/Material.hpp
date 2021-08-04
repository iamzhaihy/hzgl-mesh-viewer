#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace hzgl
{
    enum MaterialType
    {
        HZGL_PHONG_MATERIAL,
        HZGL_PBR_MATERIAL    
    };

    class Material 
    {
    public:
        MaterialType type;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;

        Material(MaterialType t = HZGL_PHONG_MATERIAL, 
                 const glm::vec3& amb  = glm::vec3(0.8f, 0.8f, 0.8f),
                 const glm::vec3& dif  = glm::vec3(0.8f, 0.8f, 0.8f),
                 const glm::vec3& spec = glm::vec3(0.5f, 0.5f, 0.5f),
                 float shin = 32.0f);
    };

    void SetupMaterial(GLuint program, const Material &material, std::string uName = "uMaterial");
    void SetupMaterialInArray(GLuint program, const Material &material, std::string uArrayName = "uMaterials", int index = 0);
} // namespace hzgl