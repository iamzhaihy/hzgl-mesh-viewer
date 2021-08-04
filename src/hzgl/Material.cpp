#include "Material.hpp"

hzgl::Material::Material(MaterialType t, const glm::vec3 &amb, const glm::vec3 &dif, const glm::vec3 &spec, float shin)
    : type(t), ambient(amb), diffuse(dif), specular(spec), shininess(shin)
{
}

static GLuint hzglLoc(GLuint program, const std::string& uName)
{
    return glGetUniformLocation(program, uName.c_str());
}

void hzgl::SetupMaterial(GLuint program, const Material &material, std::string uName)
{
    glUseProgram(program);

    glUniform3fv(hzglLoc(program, uName + ".ambient"), 1, &material.ambient[0]);
    glUniform3fv(hzglLoc(program, uName + ".diffuse"), 1, &material.diffuse[0]);
    glUniform3fv(hzglLoc(program, uName + ".specular"), 1, &material.specular[0]);
    glUniform1f(hzglLoc(program, uName + ".shininess"), material.shininess);
}

void hzgl::SetupMaterialInArray(GLuint program, const Material &material, std::string uArrayName, int index)
{
    std::string uName = uArrayName + "[" + std::to_string(index) + "]";
    SetupMaterial(program, material, uName);
}