#pragma once

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace hzgl
{
    enum MaterialType
    {
        HZGL_PHONG_MATERIAL,
        HZGL_PBR_MATERIAL,
        HZGL_ANY_MATERIAL
    };

    class Material 
    {
    public:
        MaterialType type;

        // Blinn-Phong
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;

        // PBR
        glm::vec3 albedo;
        float metallic;
        float roughness;
        float ao;

        void InitPhong(const glm::vec3& pAmbient = glm::vec3(0.78f, 0.0f, 0.0f),
                       const glm::vec3& pDiffuse = glm::vec3(0.78f, 0.0f, 0.0f),
                       const glm::vec3& pSpecular = glm::vec3(0.5f, 0.5f, 0.5f),
                       float pShininess = 32.0f);

        void InitPBR(const glm::vec3& pAlbedo = glm::vec3(0.78f, 0.0f, 0.0f),
                     float pMetallic = 0.5, float pRoughness = 0.5, float pAo = 1.0); 

        Material(MaterialType t = HZGL_PHONG_MATERIAL);
    };

    std::string MaterialTypeName(MaterialType type);
    Material CreatesSampleMaterial(MaterialType type, const std::string &name);
    void SetupMaterial(GLuint program, const Material &material, std::string uName = "uMaterial");
    void SetupMaterialInArray(GLuint program, const Material &material, std::string uArrayName = "uMaterials", int index = 0);
} // namespace hzgl