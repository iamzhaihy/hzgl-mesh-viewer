#include "Material.hpp"

hzgl::Material::Material(MaterialType t): type(t) 
{
    if (type == HZGL_PHONG_MATERIAL)
        InitPhong();
    else if (type == HZGL_PBR_MATERIAL)
        InitPBR();
}

void hzgl::Material::InitPhong(const glm::vec3 &pAmbient, const glm::vec3 &pDiffuse, const glm::vec3 &pSpecular, float pShininess) 
{
    ambient = pAmbient;
    diffuse = pDiffuse;
    specular = pSpecular;
    shininess = pShininess;
}

void hzgl::Material::InitPBR(const glm::vec3 &pAlbedo, float pMetallic, float pRoughness, float pAo) 
{
    albedo = pAlbedo;
    metallic = pMetallic;
    roughness = pRoughness;
    ao = pAo;
}

static GLuint hzglLoc(GLuint program, const std::string& uName)
{
    return glGetUniformLocation(program, uName.c_str());
}

std::string hzgl::MaterialTypeName(MaterialType type)
{
    std::string matType;

    switch (type)
    {
        case HZGL_PHONG_MATERIAL:
            matType = "Phong Material";
            break;
        case HZGL_PBR_MATERIAL:
            matType = "PBR Material";
            break;
        default:
            matType = "Unknown type";
            break;
    }

    return matType;
}

void hzgl::SetupMaterial(GLuint program, const Material& material, std::string uName)
{
    glUseProgram(program);

    if (material.type == HZGL_PHONG_MATERIAL)
    {
        glUniform3fv(hzglLoc(program, uName + ".ambient"), 1, &material.ambient[0]);
        glUniform3fv(hzglLoc(program, uName + ".diffuse"), 1, &material.diffuse[0]);
        glUniform3fv(hzglLoc(program, uName + ".specular"), 1, &material.specular[0]);
        glUniform1f(hzglLoc(program, uName + ".shininess"), material.shininess);
    }
    else if (material.type == HZGL_PBR_MATERIAL)
    {
        glUniform3fv(hzglLoc(program, uName + ".albedo"), 1, &material.albedo[0]);
        glUniform1f(hzglLoc(program, uName + ".metallic"), material.metallic);
        glUniform1f(hzglLoc(program, uName + ".roughness"), material.roughness);
        glUniform1f(hzglLoc(program, uName + ".ao"), material.ao);
    }
}

void hzgl::SetupMaterialInArray(GLuint program, const Material& material, std::string uArrayName, int index) 
{
    std::string uName = uArrayName + "[" + std::to_string(index) + "]";
    SetupMaterial(program, material, uName);
}